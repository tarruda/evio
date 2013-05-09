require 'evio'

class String
  # Strip leading whitespace from each line that is the same as the 
  # amount of whitespace on the first line of the string.
  # Leaves _additional_ indentation on later lines intact.
  def unindent
    gsub /^#{self[/\A\s*/]}/, ''
  end
end

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
        fork { Process.kill('USR1', pid) }
      end
      @flag.should eq 1
    end
  end

  describe 'file io' do

    fname = 'specs.log'
    fcontents = <<-END.unindent
      This is a stub file
        used
      for testing evio.
    END

    before :each do
      File.open(fname, 'w') do |f|
        f.puts fcontents
      end
      @io = Evio::IO.open(fname)
    end

    after :each do
      File.unlink(fname)
      @io.close
    end

    it 'can read' do
      @io.read do |err, chunk|
        chunk.should eq fcontents
      end
      Evio::start_loop
    end

    it 'can read smaller chunks' do
      chunks = []
      count = 0
      @io.read(5) do |err, chunk|
        chunks.push chunk
        count += 1
        count < 2 # only read two chunks
      end
      Evio::start_loop
      chunks.should eq ['This ', 'is a ']
    end
  end

  # describe 'stop_loop' do
  #   # FIXME for some reason(related with libev) this test make others that
  #   # follow it fail, so it must be declared last
  #   it 'breaks loop but finishes processing pending events' do
  #     Evio::set_timer(0, 10e-9) do
  #       @flag += 1
  #       @flag.should eq 1
  #       Evio::stop_loop
  #     end
  #     Evio::set_timer(0, 10e-9) do
  #       @flag += 1
  #       @flag.should eq 2
  #       Evio::set_timer(0, 10e-9) do
  #         # this will never be called since the loop will exit after
  #         # this iteration
  #         @flag += 1
  #       end
  #       Evio::stop_loop
  #     end
  #     Evio::start_loop
  #     @flag.should eq 2
  #   end
  # end
end
