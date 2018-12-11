defmodule Goof do
  @moduledoc """
  Goof - A thing that makes OpenGL windows, draws in them, and
  allows key events.
  """

  @on_load :load_native
  def load_native do
    priv = :code.priv_dir(:goof)
    if File.exists?(priv) do
      :erlang.load_nif(priv ++ '/goof', 0)
    else
      {:error, "NIF loading failed"}
    end
  end

  @doc """
  Get the version string from GLFW.
  """
  def glfw_version() do
    raise "NIF didn't happen for glfw_version()"
  end

  def init() do
    raise "NIF didn't happen for init()"
  end

  def set_points(_points) do
    raise "NIF didn't happen for set_points(...)"
  end

  def cleanup() do
    raise "NIF didn't happen for terminate()"
  end
end
