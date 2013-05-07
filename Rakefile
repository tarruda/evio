require 'rspec/core/rake_task'
require 'rake/clean'

NAME = 'evio'

file "lib/#{NAME}/#{NAME}.so" => Dir.glob("ext/#{NAME}/*{.rb,.c}") do
  Dir.chdir("ext/#{NAME}") do
    ruby "extconf.rb"
    sh "make"
  end
  cp "ext/#{NAME}/#{NAME}.so", "lib/#{NAME}"
end


RSpec::Core::RakeTask.new('spec')

# If you want to make this the default task
task :default => :spec

task :spec => "lib/#{NAME}/#{NAME}.so"

CLEAN.include('ext/**/*{.o,.log,.so}')
CLEAN.include('ext/**/Makefile')
CLOBBER.include('lib/**/*.so')
