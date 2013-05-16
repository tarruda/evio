
module EvIO
  class Stream
    include Emitter

    def initialize(inner)
      10.times { puts inner }
      @inner = inner
    end

  end
end
