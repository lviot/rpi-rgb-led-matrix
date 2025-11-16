defmodule RpiRgb do
  @on_load :load_nifs

  def load_nifs do
    :erlang.load_nif(~c"./rgbmatrix_nif", 0)
  end

  def led_matrix_create(_a, _b, _c) do
    raise "NIF led_matrix_create/3 not implemented"
  end

  def led_matrix_create_from_options(_a) do
    raise "NIF led_matrix_create_from_options/1 not implemented"
  end

  def led_matrix_create_from_options_and_rt_options(_a, _b) do
    raise "NIF led_matrix_create_from_options_and_rt_options/1 not implemented"
  end

  def create_options(_a) do
    raise "NIF create_options/1 not implemented"
  end

  def create_rt_options(_a) do
    raise "NIF create_rt_options/1 not implemented"
  end

  def led_matrix_create_offscreen_canvas(_a) do
    raise "NIF led_matrix_create_offscreen_canvas/1 not implemented"
  end

  def led_canvas_get_size(_a) do
    raise "NIF led_canvas_get_size/1 not implemented"
  end

  def led_canvas_set_pixel(_a, _b, _c, _d, _e, _f) do
    raise "NIF led_canvas_set_pixel/6 not implemented"
  end

  def led_canvas_clear(_a) do
    raise "NIF led_canvas_clear/1 not implemented"
  end

  def led_canvas_fill(_a, _b, _c, _d) do
    raise "NIF led_canvas_fill/4 not implemented"
  end

  def led_matrix_swap_on_vsync(_a, _b) do
    raise "NIF led_matrix_swap_on_vsync/2 not implemented"
  end

  def led_matrix_get_brightness(_a) do
    raise "NIF led_matrix_get_brightness/1 not implemented"
  end

  def led_matrix_set_brightness(_a, _b) do
    raise "NIF led_matrix_set_brightness/2 not implemented"
  end
end

defmodule Run do
  import Bitwise
  # sudo ../../examples-api-use/c-example --led-rows=32 --led-cols=64 --led-gpio-mapping=adafruit-hat --led-slowdown-gpio=3
  def run do
    opts = RpiRgb.create_options([rows: 32, cols: 64, hardware_mapping: ~c"adafruit-hat"])
    rt_opts = RpiRgb.create_rt_options([gpio_slowdown: 3])
    matrix = RpiRgb.led_matrix_create_from_options_and_rt_options(opts, rt_opts)
    offscreen_canvas = RpiRgb.led_matrix_create_offscreen_canvas(matrix)
    {width, height} = RpiRgb.led_canvas_get_size(offscreen_canvas)

    for i <- 0..999 do
      for x <- 0..width do
        for y <- 0..height do
          RpiRgb.led_canvas_set_pixel(offscreen_canvas, x, y, Bitwise.band(i, 0xff), x, y)
        end
      end

      offscreen_canvas = RpiRgb.led_matrix_swap_on_vsync(matrix, offscreen_canvas);
    end

    # RpiRgb.led_matrix_delete(matrix)
  end
end