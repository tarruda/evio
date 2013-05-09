
module EvIO
  class Stream
    def read_sync(chunk_size)
      rv = nil
      fiber = Fiber::current
      read(chunk_size) do |err, chunk|
        rv = chunk
        fiber.resume
        false
      end
      Fiber.yield
      rv
    end
  end
end
