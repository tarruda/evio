
module EvIO
  class Stream
    include Emitter

    def initialize(inner)
      @inner = inner
    end


  end
end
