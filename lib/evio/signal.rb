
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
      rv = save_handler(block, event, *args)
      if not @subscribed[event]
        # only need one libuv handle per signal
        @subscribed[event] =
          subscribe_signal(@signal_table[event], event, block)
      end
      rv
    end

    # def handler_disabled(handler_array, event)
    #   if handler_array.length == 0 and @subscribed[event]
    #     @subscribed.delete(event)
    #     # GC::start
    #   end
    # end

    def stop_handle?(handler_array, event, *args)
      false
    end
  end

  original_verbose, $VERBOSE = $VERBOSE, nil
  Signal = Signal.instance
  $VERBOSE = original_verbose
end


