module EvIO
  class Signal
    include Emitter
    include Singleton

    def initialize
      @subscribed = {}
      @signal_table = {}
      ::Signal.list.each do |k, v|
        @signal_table[k.to_sym] = v
      end
    end

    def remove_default_traps
      # Remove default traps set by the ruby interpreter
      ::Signal.list.values.each do |s|
        ::Signal.trap s, 'IGNORE'
      end
    end

    protected
    def subscribe(block, event, *args)
      signum = @signal_table[event]
      if not signum
        raise ArgumentError, 'invalid signal'
      end
      rv = save_handler(block, event, *args)
      @handles ||= {}
      if not @handles[event]
        handle = signal_handle_new(signum) do
          process_handle_cb(@handlers[event], handle, event, *args)
        end
        handle = HandleWrap.new(handle, :signal)
        @handles[event] = handle
      end
      rv
    end

    def stop_handle?(handler_array, *args)
      handler_array.length == 0
    end
  end

  original_verbose, $VERBOSE = $VERBOSE, nil
  Signal = Signal.instance
  $VERBOSE = original_verbose
end


