require 'evio'
require 'fiber'

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

  describe 'on_timeout' do
    it 'only executes block on event loop iteration' do
      EvIO::on_timeout(0, 0) do
        @flag = 1
      end
      @flag.should eq 0
      EvIO::start_looping
      @flag.should eq 1
    end

    it 'stops repeating when false is returned from block' do
      EvIO::on_timeout(0, 10e-9) { @flag += 1; false if @flag == 5 }
      EvIO::start_looping
      @flag.should eq 5
    end   

    it 'accepts only numeric arguments' do
      expect { EvIO::on_timeout('', 0) { } }.to raise_error(ArgumentError)
      expect { EvIO::on_timeout(true, 0) { } }.to raise_error(ArgumentError)
      expect { EvIO::on_timeout(0, '') { } }.to raise_error(ArgumentError)
    end

    it 'raises error when no block is passed' do
      expect { EvIO::on_timeout(0, 0) }.to raise_error(ArgumentError)
    end
  end

  describe 'trap' do
    it 'runs block when signal is received' do
      signals = Signal.list
      EvIO::on_signal(signals['USR1']) { @flag = 1; false }
      @flag.should eq 0
      pid = Process.pid
      EvIO::start_looping do
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
      @file = EvIO::File.open(fname)
    end

    after :each do
      File.unlink(fname)
      @file.close
    end

    it 'can read' do
      @file.read do |err, chunk|
        chunk.should eq fcontents
      end
      EvIO::start_looping
    end

    it 'can read smaller chunks' do
      chunks = []
      count = 0
      @file.read(5) do |err, chunk|
        chunks.push chunk
        count += 1
        count < 2 # only read two chunks
      end
      EvIO::start_looping
      chunks.should eq ['This ', 'is a ']
    end

    it 'can read synchronously inside fibers' do
      chunks = []
      EvIO::start_looping do
        fiber = Fiber.new do
          chunks.push @file.read_sync(5)
          chunks.push @file.read_sync(5)
        end
        fiber.resume
      end
      chunks.should eq ['This ', 'is a ']
    end
  end

end
