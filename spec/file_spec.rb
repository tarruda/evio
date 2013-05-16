require 'evio'
require 'fiber'


describe 'File::' do

  fname = 'spec/fixtures/file_spec.txt'

  it 'can open for reading' do
    file = EvIO::File::open(fname) do |err, stream|
    end
    EvIO::start
  end

  # it 'can read' do
  #   @file.read do |err, chunk|
  #     chunk.should eq fcontents
  #   end
  #   EvIO::start
  # end

  # it 'can read smaller chunks' do
  #   chunks = []
  #   count = 0
  #   @file.read(5) do |err, chunk|
  #     chunks.push chunk
  #     count += 1
  #     count < 2 # only read two chunks
  #   end
  #   EvIO::start
  #   chunks.should eq ['This ', 'is a ']
  # end

  # it 'can read synchronously inside fibers' do
  #   chunks = []
  #   EvIO::start do
  #     fiber = Fiber.new do
  #       chunks.push @file.read_sync(5)
  #       chunks.push @file.read_sync(5)
  #     end
  #     fiber.resume
  #   end
  #   chunks.should eq ['This ', 'is a ']
  # end
end
