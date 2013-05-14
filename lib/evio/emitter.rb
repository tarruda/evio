module EvIO
  class Handler
    attr_reader :block

    def initialize(handlers, block, emitter, event)
      @block = block
      @handlers = handlers
      @emitter = emitter
      @event = event
    end

    def disable
      @handlers.delete(self)
      # @emitter.send(:handler_disabled, @handler_array, @event)
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
      handler = Handler.new(@handlers[event], block, self, event)
      @handlers[event].push(handler)
      handler
    end

    def process_event(handler_array, event, args)
      len = handler_array.length
      i = 0
      while i < len
        handler = handler_array[i]
        block = handler.block
        result = block.call(*args)
        if disable_handler?(result, event, *args)
          handler.disable()
          len -= 1
        else
          i += 1
        end
      end
      :stop if len == 0 or stop_handle?(handler_array, event, args)
    end

    def stop_handle?(*args)
      true
    end

    def disable_handler?(result, *args)
      result == :disable
    end

    # def handler_disabled(handler_array, event)
    # end
  end
end
