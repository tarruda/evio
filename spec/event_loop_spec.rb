require 'evio'

include Evio

describe 'set_timeout' do

  before :each do
    @flag = 0
  end

  it 'should only execute block on event loop iteration' do
    set_timeout(0, 0) do
      @flag = 1
    end
    @flag.should eq 0
    start_loop
    @flag.should eq 1
  end

  it 'should accept only numeric arguments' do
    expect { set_timeout('', 0) { } }.to raise_error(ArgumentError)
    expect { set_timeout(true, 0) { } }.to raise_error(ArgumentError)
    expect { set_timeout(0, '') { } }.to raise_error(ArgumentError)
  end

  it 'should raise error when no block is passed' do
    expect { set_timeout(0, 0) }.to raise_error(ArgumentError)
  end

end
