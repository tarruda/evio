require 'singleton'

module EvIO
  class Timer
    include Emitter
    include Singleton

    protected
    def subscribe(block, event, *args)
      handler_array = []
      handler = Handler.new(handler_array, block)
      handler_array.push(handler)
      case event
      when :tick
        emit_tick(handler_array)
      when :timeout
        delay = args[0]
        if not delay.is_a? Numeric
          raise ArgumentError, 'delay argument must be a number'
        end
        if delay <= 0
          raise ArgumentError, 'delay must be greater than 0'
        end
        emit_timeout(handler_array, delay)
      when :interval
        interval = args[0]
        delay = args[1]
        if not interval.is_a? Numeric
          raise ArgumentError, 'interval argument must be a number'
        end
        if interval <= 0
          raise ArgumentError, 'interval must be greater than 0'
        end
        delay = interval if not delay.is_a? Numeric
        emit_interval(handler_array, interval, delay)
      end
    end

    def stop_handle?(handler_array, event, *args)
      event == :tick or event == :timeout
    end
  end

  TIMER = Timer.instance()
end

