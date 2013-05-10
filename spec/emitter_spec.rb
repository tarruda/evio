require 'evio'

class TestEmitter
  include EvIO::Emitter

  def do_something
    emit :some_event
  end
end

describe 'Emitter' do
  before :each do
    @emitter = TestEmitter.new
    @flag = 0
  end

  it 'provides arbitrary event notification' do
    EvIO::start do
    @emitter.on :some_event do
      @flag = 1
    end
    @flag.should eq 0
      @emitter.do_something
    end
    @flag.should eq 1
  end
end
