require 'evio'

class TestEmitter
  include EvIO::Emitter

  def trigger_event
    emit(:event, 'arg1', 'arg2')
  end
end

class OverrideSubscribeEmitter < TestEmitter
  attr_reader :flag

  protected
  alias :subscribe_super :subscribe

  def subscribe(block, event, *args)
    @flag = args.join(',')
    rv = subscribe_super(block, event, *args)
    emit(event)
    rv
  end
end

describe 'Emitter mixin:' do
  before :each do
    @emitter = TestEmitter.new
    @flag = 0
  end

  describe "'on' method" do
    it 'invokes the passed block only when the event is triggered' do
      EvIO::start do
        @emitter.on :event do
          @flag = 1
        end
        @flag.should eq 0
        @emitter.trigger_event()
      end
      @flag.should eq 1
    end
  end

  describe "handler object returned by the 'on' method" do
    it "stops handling the event by calling 'disable'" do
      EvIO::start do
        handler = @emitter.on :event do
          @flag = 1
        end
        @flag.should eq 0
        handler.disable()
        @emitter.trigger_event()
      end
      @flag.should eq 0
    end
  end

  describe "block passed to the 'on' method" do
    it 'stops handling the event when :disable is returned' do
      EvIO::start do
        @emitter.on :event do
          @flag = 1
          :disable
        end
        @flag.should eq 0
        @emitter.trigger_event()
        @emitter.trigger_event() # handler is already canceled
      end
      @flag.should eq 1
    end
  end

  describe "protected 'emit' method" do
    it 'runs the block passed in the event loop' do
      EvIO::start do
        @emitter.on :event do
          @flag = 1
        end
        @flag.should eq 0
        @emitter.trigger_event()
        @flag.should eq 0 # only sets the flag in the next iteration
      end
      @flag.should eq 1
    end

    it 'can pass arguments to event handlers' do
      EvIO::start do
        @emitter.on :event do |a1, a2|
          @flag = "#{a1},#{a2}"
        end
        @flag.should eq 0
        @emitter.trigger_event()
      end
      @flag.should eq 'arg1,arg2'
    end
  end

  describe "protected 'subscribe' method" do
    before :each do
      @emitter = OverrideSubscribeEmitter.new
    end

    it 'can be used to execute actions when someone subscribe to events' do
      EvIO::start do
        @emitter.on :event, 1, 2, 3 do
          @flag = 1
        end
        @emitter.flag.should eq '1,2,3'
      end
      @flag.should eq 1
    end
  end
end
