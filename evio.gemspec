Gem::Specification.new do |s|
  s.name        = 'evio'
  s.version     = '0.0.0'
  s.date        = '2013-05-07'
  s.summary     = 'Use libev for ruby IO methods'
  s.authors     = ['Thiago de Arruda']
  s.email       = 'tpadilha84@gmail.com'
  s.files       = Dir.glob('lib/**/*.rb') + Dir.glob('ext/**/*.{c,h,rb}')
  s.extensions  = ['ext/hola/extconf.rb']
  s.homepage    = 'http://rubygems.org/gems/evio'
  s.add_development_dependency 'rspec'
end
