ENV['RC_ARCHS'] = '' if RUBY_PLATFORM =~ /darwin/

require 'mkmf'

HEADER_DIRS = [
  '/opt/local/include',
  '/usr/local/include',
  Config::CONFIG['includedir'],
  '/usr/include',
]

LIB_DIRS = [
  '/opt/local/lib',
  '/usr/local/lib',
  Config::CONFIG['libdir'],
  '/usr/lib',
]

dir_config('ev', HEADER_DIRS, LIB_DIRS)

unless find_header('ev.h')
  abort 'cannot find libev headers, install it before continuing'
end

unless find_library('ev', 'ev_run')
  abort 'cannot find libev'
end

create_makefile('evio/evio')
