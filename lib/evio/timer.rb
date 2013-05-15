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
      case event
      when :tick
        timer_args = [0, 0]
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

  original_verbose, $VERBOSE = $VERBOSE, nil
  Timer = Timer.instance
  $VERBOSE = original_verbose
end

