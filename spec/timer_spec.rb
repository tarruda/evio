require 'evio'

describe 'Timer' do

  before :each do
    @flag = 0
  end

  describe 'on :tick' do
    it 'executes block once on next event loop iteration' do
      EvIO::start do
        EvIO::TIMER.on :tick do
          @flag = 1
        end
        @flag.should eq 0
      end
      @flag.should eq 1
    end
  end

  # describe 'on :timeout' do
  #   it 'executes block once after the time has ellapsed' do
  #     EvIO::TIMER.on :timeout, 0.1 do
  #       @flag = 1
  #     end
  #     @flag.should eq 0
  #     start = Time.now
  #     EvIO::start
  #     ellapsed = Time.now - start
  #     ellapsed.should be > 0.09 # 10 ms margin should be enough
  #     @flag.should eq 1
  #   end
  # end

  # describe 'on_interval' do
  #   it 'stops repeating when false is returned from block' do
  #     EvIO::on_interval(0.01) { @flag += 1; false if @flag == 5 }
  #     EvIO::start
  #     @flag.should eq 5
  #   end

  #   it 'accepts delay before first run as second argument' do
  #     EvIO::on_interval(1, 0.01) { @flag += 1; false }
  #     EvIO::start
  #     @flag.should eq 1
  #   end

  #   it 'accepts only numeric arguments' do
  #     expect { EvIO::on_interval('', 0) { } }.to raise_error(ArgumentError)
  #     expect { EvIO::on_interval(true, 0) { } }.to raise_error(ArgumentError)
  #     expect { EvIO::on_interval(0, '') { } }.to raise_error(ArgumentError)
  #   end

  #   it "does not accept zero interval" do
  #     expect { EvIO::on_interval(0) }.to raise_error(ArgumentError)
  #   end
  # end

  # describe 'on_signal' do
  #   it 'runs block when signal is received' do
  #     signals = Signal.list
  #     EvIO::on_signal(signals['USR1']) { @flag = 1; false }
  #     @flag.should eq 0
  #     pid = Process.pid
  #     EvIO::start do
  #       fork { Process.kill('USR1', pid) }
  #     end
  #     @flag.should eq 1
  #   end
  # end
end
