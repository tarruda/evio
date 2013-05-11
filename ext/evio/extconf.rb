require 'mkmf'
require 'digest'
require 'open-uri'
require 'rubygems/package'
require 'zlib'


def build_libuv
  sha1sum = 'fe8777fdf0e719a4570746594ca3749e8eed3b4e'
  downloaded_libuv = '../libuv.tar.gz'
  extracted_libuv = '../libuv'
  if not FileTest::directory? extracted_libuv
    if not FileTest::readable? downloaded_libuv
      print 'downloading libuv ...' 
      begin
        sha1 = Digest::SHA1.new
        local = open(downloaded_libuv, 'wb')
        open('https://github.com/joyent/libuv/archive/v0.10.5.tar.gz') do |remote|
          while (chunk = remote.read(4096)) != nil
            sha1 << chunk
            local << chunk
          end
        end
        digest = Digest::hexencode(sha1.digest())
        if digest != sha1sum
          raise Exception, 'Checksum does not match!'
        end
        puts ' done!'
      rescue Exception => e
        puts 'Error ocurred:'
        puts e.to_s
      ensure
        local.close()
      end
    end
    Dir::mkdir(extracted_libuv)
    Gem::Package::TarReader.new(Zlib::GzipReader.open(downloaded_libuv)).each do |entry|
      full_path = "#{extracted_libuv}/#{entry.full_name.sub(/^.+?\//, '')}"
      if entry.directory?
        FileUtils::mkdir_p full_path
      else
        puts "extracting to #{full_path} ..."
        File::open(full_path, 'wb') do |f|
          f.write(entry.read)
        end
      end
      pwd = Dir::pwd
      Dir::chdir extracted_libuv
      # system('make libuv.so')
      system('make')
      Dir::chdir pwd
    end
  end
end


HEADER_DIRS = [
  '/opt/local/include',
  '/usr/local/include',
  RbConfig::CONFIG['includedir'],
  '/usr/include',
  '../libuv/include'
]

LIB_DIRS = [
  '/opt/local/lib',
  '/usr/local/lib',
  RbConfig::CONFIG['libdir'],
  '/usr/lib',
  '../libuv'
]

build_libuv

dir_config('uv', HEADER_DIRS, LIB_DIRS)

if not find_header('uv.h')
  abort 'cannot find libuv, will download and build statically!'
end

if not find_library('uv', 'uv_run')
  abort 'cannot find libuv'
end

create_makefile('evio/evio')
