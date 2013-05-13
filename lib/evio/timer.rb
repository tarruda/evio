require 'singleton'

module EvIO
  class Timer
    include Emitter
    include Singleton

    protected
    alias :subscribe_super :subscribe

    def subscribe(block, event, *args)
      case event
      when :tick
        subscribe_super(block, event)
        emit(:tick)
      when :timeout
        emit_timeout(block, args[0])
      end
    end

    def disable_handler?(result, event, *args)
      event == :tick
    end
  end

  TIMER = Timer.instance()
end

