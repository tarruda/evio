require 'singleton'

module EvIO
  class Timer
    include Emitter
    include Singleton

    protected
    def save_handler(block, event, *args)
      emit(block, event, *args)
    end

    def emit(block, event, *args)
      if event == :tick
        @tick_array ||= []
        handler = Handler.new(@tick_array, block)
        @tick_array.push(handler)
        if not @tick_handle
          @tick_handle = idle_handle_new() do
            process_handle_cb(@tick_array, @tick_handle, event)
            @tick_array = []
            @tick_handle_disabled = true
          end
          @tick_handle = HandleWrap.new(@tick_handle, :idle)
          @tick_handle_disabled = true
        end
        if @tick_handle_disabled
          @tick_handle.enable() 
          @tick_handle_disabled = false
        end
        handler
      else
        case event
        when :timeout
          delay = args[0]
          if not delay.is_a?(Numeric)
            raise ArgumentError, 'timeout delay must be greater than 0'
          end
          timer_args = [delay, 0]
        when :interval
          interval = args[0]
          delay = args[1]
          if not args[0].is_a?(Numeric)
            raise ArgumentError, 'interval delay must be greater than 0'
          end
          if delay
            if not delay.is_a?(Numeric)
              raise ArgumentError, 'interval delay must be greater than 0'
            end
          else
            delay = interval
          end
          timer_args = [delay, interval]
        end
        handle = timer_handle_new(*timer_args) do
          result = block.call()
          handle.disable() if result == :disable or event != :interval
        end
        handle = HandleWrap.new(handle, :timer)
        handle.enable()
        handle
      end
    end
  end

  original_verbose, $VERBOSE = $VERBOSE, nil
  Timer = Timer.instance
  $VERBOSE = original_verbose
end

