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


# describe 'file io' do

#   fname = 'specs.log'
#   fcontents = <<-END.unindent
#       This is a stub file
#         used
#       for testing evio.
#   END

#   before :each do
#     File.open(fname, 'w') do |f|
#       f.puts fcontents
#     end
#     @file = EvIO::File.open(fname)
#   end

#   after :each do
#     File.unlink(fname)
#     @file.close
#   end

#   it 'can read' do
#     @file.read do |err, chunk|
#       chunk.should eq fcontents
#     end
#     EvIO::start
#   end

#   it 'can read smaller chunks' do
#     chunks = []
#     count = 0
#     @file.read(5) do |err, chunk|
#       chunks.push chunk
#       count += 1
#       count < 2 # only read two chunks
#     end
#     EvIO::start
#     chunks.should eq ['This ', 'is a ']
#   end

#   it 'can read synchronously inside fibers' do
#     chunks = []
#     EvIO::start do
#       fiber = Fiber.new do
#         chunks.push @file.read_sync(5)
#         chunks.push @file.read_sync(5)
#       end
#       fiber.resume
#     end
#     chunks.should eq ['This ', 'is a ']
#   end
# end
