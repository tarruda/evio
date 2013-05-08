require 'evio'

include Evio

describe 'event loop' do

  describe 'set_timer' do

    before :each do
      @flag = 0
    end

    it 'only executes block on event loop iteration' do
      set_timer(0, 0) do
        @flag = 1
      end
      @flag.should eq 0
      start_loop
      @flag.should eq 1
    end

    it 'stops repeating when false is returned from block' do
      set_timer(0, 10e-9) do
        @flag += 1
        false if @flag == 5
      end
      start_loop
      @flag.should eq 5
    end   

    it 'accepts only numeric arguments' do
      expect { set_timer('', 0) { } }.to raise_error(ArgumentError)
      expect { set_timer(true, 0) { } }.to raise_error(ArgumentError)
      expect { set_timer(0, '') { } }.to raise_error(ArgumentError)
    end

    it 'raises error when no block is passed' do
      expect { set_timer(0, 0) }.to raise_error(ArgumentError)
    end

  end

end
