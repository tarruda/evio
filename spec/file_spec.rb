require 'evio'
require 'fiber'


describe 'File::' do

  fname = 'spec/fixtures/file_spec.txt'

  it 'opens files as streams' do
    file = EvIO::File::open(fname) do |err, stream|
      err.should eq(nil)
      stream.should be_a(EvIO::Stream)
    end
    EvIO::start
  end

  it 'returns error on inexistent file' do
    file = EvIO::File::open('nofile') do |err, stream|
      puts err
      err.should be_a(SystemCallError)
      stream.should eq(nil)
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
