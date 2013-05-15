module EvIO
  class HandleWrap
    def initialize(handle, type)
      @handle = handle
      @type = type
    end

    def disable
      case @type
      when :idle
        disable_idle(@handle)
      when :timer
        disable_timer(@handle)
      when :signal
        disable_signal(@handle)
      end
    end
  end

  class Handler
    attr_reader :block

    def initialize(handler_array, block)
      @block = block
      @handler_array = handler_array
    end

    def disable
      @handler_array.delete(self)
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

    def emit(event, *args)
      return false if not @handlers or not @handlers[event]
      @handles ||= {}
      if not @handles[event]
        handle = idle_handle_new() do
          process_handle_cb(@handlers[event], handle, event, *args)
        end
        handle = HandleWrap.new(handle, :idle)
        @handles[event] = handle
      end
      true
    end

    def process_handle_cb(handler_array, handle, event, *args)
      len = handler_array.length
      i = 0
      while i < len
        handler = handler_array[i]
        block = handler.block
        result = block.call(*args)
        if result == :disable
          handler.disable()
          len -= 1
        else
          i += 1
        end
      end
      if stop_handle?(handler_array, event, args)
        handle.disable()
        @handles.delete(event)
      end
    end

    def stop_handle?(*args)
      true
    end
  end
end
