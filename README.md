# Goof

"Goof" stands for GLFW-based view/UI For Elixir.

Initial functionality:

 - Open a GUI window.
 - Send a list of points to render in the window.

Render list of points:

 - A Point is {Posn, Color, Size}
 - A Posn is {X, Y} where both are in [-1.0, 1.0]
 - A Color is {R, G, B, A} where all are in [0.0, 1.0]

 - List to render is just a list of points.
 - This is the bottleneck, since we'll have to retranslate on
   each frame.

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `goof` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:goof, "~> 0.1.0"}
  ]
end
```

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/goof](https://hexdocs.pm/goof).

