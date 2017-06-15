-module(fasthash).


-export([init/0,fasthash64/1]).


init() ->
    erlang:load_nif("nif/fasthash", 0).

fasthash64(String) ->
  fasthash644erl(length(String), String).

fasthash644erl(_Size, _String) ->
    "NIF library not loaded".

