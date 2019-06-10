(* ocamlbuild -pkg cohttp-lwt-unix task.native *)
open Lwt
open Cohttp
open Cohttp_lwt_unix

(* just copy-paste some code for state monad *)
module type MONAD =
sig
  type 'a t
  val bind : 'a t -> ('a -> 'b t) -> 'b t
  val return : 'a -> 'a t
end

module type STATE =
sig
  type t
  val empty : t
end

module type STATE_MONAD =
  functor(State : STATE) ->
  sig
    include MONAD
    val access : 'a t -> 'a
    val put : State.t -> unit t
    val get : State.t t
  end

module StateMonad : STATE_MONAD =
  functor(State : STATE) ->
  struct
    type state = State.t
    type 'a t = state -> ('a * state)
    let bind m f = fun s -> match m s with 
      | (x, s') -> f x s'
    let return a = fun s -> (a, s)
    let access m = match m State.empty with
      | (x, s) -> x
    let put s = fun _ -> ((), s)
    let get = fun s -> (s, s)
  end

module SessionState = StateMonad(
  struct
    type t = string
    let empty = ""
  end
)

let working_dir = "/home/user/ocaml/"

let response body =
  Server.respond_string ~status:`OK ~body ()

let unknown =
  Server.respond_string ~status:`Not_found ~body:"" ()

let forbidden =
  Server.respond_string ~status:`Forbidden ~body:"" ()

let default_user = "NoName"

(* should return static files. not implemented. too lazy *)
let static _ _ _ =
  response "<html><body>It works!</body></html>"

let digest pass =
    Digest.to_hex (Digest.string pass)

let whoami = fun _ -> SessionState.get

let get_handler uri =
  try
    let pos = String.index uri '?' in
    let len = String.length uri in
    let args = String.sub uri (pos+1) (len-pos-1) in
    let ss = String.split_on_char '/' (String.sub uri 0 pos) in
    match ss with
    | _::_::_::a::others -> (a,args)
    | _ -> ("","")
  with _ -> ("static","index.html")

(* read first line only. to avoid possible leaks *)
let readfile path =
  try
  let chan = open_in path in
  let line=input_line chan in
  let _ = close_in chan in 
  line
  with _ -> ""
  
let writefile path cont =
  try
  let chan = open_out path in
  let _ = Printf.fprintf chan "%s" cont in
  let _ = close_out chan in
  let _ = Unix.chmod path 0o600 in
  true
  with _ -> false

let register req body args =
  let path = "user/"^args in
  match Sys.file_exists path with
  | false ->
    let res = body |> Cohttp_lwt.Body.to_string >|= (fun body ->
      let d = digest body in
      writefile path d) in
    res >>= (fun res ->
      match res with
      | true -> response "register succeeded"
      | false -> response "register failed")
  | _ -> response "user already exists"

let login = fun req body args _ ->
  let path = "user/"^args in
  let line = readfile path in
  match line with
  | "" -> SessionState.return ()
  | _ ->
    let d = digest body in
    let res = String.equal d line in
    match res with
    | true -> SessionState.put (digest args)
    | false -> SessionState.return ()

let test_login req body args =
  let sess = SessionState.put default_user in
  let res = body |> Cohttp_lwt.Body.to_string >|= (fun body ->
    SessionState.bind sess (login req body args)) in
  res >>= (fun res ->
    let user = SessionState.bind res whoami in
    match (SessionState.access user) with
    | name when name = default_user -> response "login failed"
    | _ -> response "login succeeded")

let real_login check user cont req body args =
  match check,args with
  | true, name when name = default_user -> 
    let res = SessionState.put default_user in
    SessionState.bind res (fun _ -> SessionState.return (cont^"login|"))
  | true, name when name = (SessionState.access user) ->
    let res = SessionState.put name in
    SessionState.bind res (fun _ -> SessionState.return (cont^"login|"))
  | _ -> 
    let res = SessionState.bind (SessionState.put default_user) (login req body args) in
    let cur_user = SessionState.bind res whoami in
    match (SessionState.access cur_user) with
    | name when name = default_user -> SessionState.bind res (fun _ -> SessionState.return cont)
    | _ -> SessionState.bind res (fun _ -> SessionState.return (cont^"login|"))

let load = fun req body args user ->
  let path = "db/"^user^args in
  let line = readfile path in
  SessionState.return line

let store = fun req body args user ->
  let path = "db/"^user^args in
  let res = writefile path body in
  match res with
  | true -> SessionState.return "true"
  | false -> SessionState.return "false"

let default_load req body args =
  let sess = SessionState.return default_user in
  let res = SessionState.bind sess (load req body args) in
  match (SessionState.access res) with
  | "" -> unknown
  | line -> response line

let default_store req body args =
  let sess = SessionState.return default_user in
  let res = body |> Cohttp_lwt.Body.to_string >|= (fun body ->
    SessionState.bind sess (store req body args)) in
  res >>= (fun res ->
    match (SessionState.access res) with
    | "true" -> response "success"
    | _ -> forbidden)

let merge = fun name body prev ->
  match body with
  | "" -> SessionState.return (prev^name^":[ERROR]|")
  | _ -> SessionState.return (prev^name^":"^body^"|")

let check_default sess =
  let user = SessionState.bind sess whoami in
  match (SessionState.access user) with
  | name when name = default_user -> true
  | _ -> false

let batch req body args =
  body |> Cohttp_lwt.Body.to_string >>= (fun body ->
    let ori_sess = SessionState.put default_user in
    let ss = String.split_on_char ':' body in
    let res = List.fold_left (fun sess cmd -> 
      let is_default = check_default sess in
      let cont = SessionState.access sess in
      match String.split_on_char '?' cmd with
      | "login"::args::body::others -> 
        let out = match is_default with
        | true -> real_login false (whoami sess) cont req body args
        | false -> real_login true (whoami sess) cont req body args
        in
        out
      | "logout"::args::body::others -> 
        let out = match is_default with
        | true -> sess
        | false -> SessionState.bind ori_sess (fun _ -> SessionState.return (cont^"logout|"))
        in
        out
      | "load"::args::body::others ->
        let user = SessionState.bind sess whoami in
        let line = SessionState.bind user (load req body args) in
        SessionState.bind sess (merge args (SessionState.access line))
      | "store"::args::body::others ->
        let user = SessionState.bind sess whoami in
        let res = SessionState.bind user (store req body args) in
        let out = match (SessionState.access res) with
        | "true" -> SessionState.bind sess (fun _ -> SessionState.return (cont^"store|"))
        | _ -> sess
        in
        out
      | _ -> sess
    ) (SessionState.bind ori_sess (fun _ -> SessionState.return "")) ss in
    response (SessionState.access res))

let server =
  let callback _conn req body =
    let uri = req |> Request.uri |> Uri.to_string in
    let (handler,args) = get_handler uri in
    match handler with
    | "register" -> register req body args
    | "login" -> test_login req body args
    | "load" -> default_load req body args 
    | "store" -> default_store req body args 
    | "static" -> static req body args
    | "batch" -> batch req body args
    | _ -> unknown 
  in
  let () = Unix.chdir working_dir in
  Server.create ~mode:(`TCP (`Port 8000)) (Server.make ~callback ())

let () = ignore (Lwt_main.run server)
