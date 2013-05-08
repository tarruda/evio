
module Evio

  module Signal

    def clear_signal_traps
      # Remove all default handlers since they cause segfault
      # with libev handlers
      Signal.list.values.each do |s|
        Signal.trap s, 'IGNORE'
      end
    end

  end

end


