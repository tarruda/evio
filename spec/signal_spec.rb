# require 'evio'

# describe 'Signal' do

#   before :each do
#     @flag = 0
#   end

#   describe 'signal emitter' do
#     it 'runs block when signal is received' do
#       EvIO::start do
#         EvIO::Signal.on(:USR1) do
#           @flag = 1
#           :disable
#         end
#         @flag.should eq 0
#         pid = Process.pid
#         fork { Process.kill('USR1', pid); }
#       end
#       @flag.should eq 1
#     end
#   end
# end
