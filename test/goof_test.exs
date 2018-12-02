defmodule GoofTest do
  use ExUnit.Case
  doctest Goof

  test "greets the world" do
    assert Goof.hello() == :world
  end
end
