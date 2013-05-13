module EvIO
  class Handler
    attr_reader :block

    def initialize(handlers, block)
      @block = block
      @handlers = handlers
    end

    def disable
      @handlers.delete(self)
    end
  end

  module Emitter
    def on(*args, &block)
      raise ArgumentError, 'an event is required' if args.length < 1
      raise ArgumentError, 'a block is required' if not block_given?
      subscribe(block, *args)
    end

    protected
    def subscribe(block, event, *args)
      save_handler(block, event, *args)
    end

    def save_handler(block, event, *args)
      @handlers ||= {}
      @handlers[event] ||= []
      handler = Handler.new(@handlers[event], block)
      @handlers[event].push(handler)
      handler
    end

    def process_event(event, args)
      return if not @handlers or @handlers.length == 0
      i = 0
      len = @handlers.length
      while i < len
        handler = @handlers[event][i]
        block = handler.block
        result = block.call(*args)
        if disable_handler?(result, event, *args)
          @handlers.delete(handler)
          len -= 1
        else
          i += 1
        end
      end
      :stop # disables the libuv handle
    end

    def disable_handler?(result, *args)
      result == :disable
    end
  end
end
