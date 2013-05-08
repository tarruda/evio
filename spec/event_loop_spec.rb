require 'evio'


describe 'event loop' do
  before :each do
    @flag = 0
  end

  describe 'set_timer' do
    it 'only executes block on event loop iteration' do
      Evio::set_timer(0, 0) do
        @flag = 1
      end
      @flag.should eq 0
      Evio::start_loop
      @flag.should eq 1
    end

    it 'stops repeating when false is returned from block' do
      Evio::set_timer(0, 10e-9) { @flag += 1; false if @flag == 5 }
      Evio::start_loop
      @flag.should eq 5
    end   

    it 'accepts only numeric arguments' do
      expect { Evio::set_timer('', 0) { } }.to raise_error(ArgumentError)
      expect { Evio::set_timer(true, 0) { } }.to raise_error(ArgumentError)
      expect { Evio::set_timer(0, '') { } }.to raise_error(ArgumentError)
    end

    it 'raises error when no block is passed' do
      expect { Evio::set_timer(0, 0) }.to raise_error(ArgumentError)
    end
  end

  describe 'trap' do
    it 'runs block when signal is received' do
      Evio::Signal::trap('USR1') { @flag = 1; false }
      @flag.should eq 0
      pid = Process.pid
      Evio::start_loop do
        fork { Process.kill("USR1", pid) }
      end
      @flag.should eq 1
    end
  end

  describe 'stop_loop' do
    # FIXME for some reason(related with libev) this test make others that
    # follow it fail, so it must be declared last
    it 'breaks loop but finishes processing pending events' do
      Evio::set_timer(0, 10e-9) do
        @flag += 1
        @flag.should eq 1
        Evio::stop_loop
      end
      Evio::set_timer(0, 10e-9) do
        @flag += 1
        @flag.should eq 2
        Evio::set_timer(0, 10e-9) do
          # this will never be called since the loop will exit after
          # this iteration
          @flag += 1
        end
        Evio::stop_loop
      end
      Evio::start_loop
      @flag.should eq 2
    end
  end
end
