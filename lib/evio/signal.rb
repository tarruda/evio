
# Remove all default handlers since they cause segfault due to libev handlers
Signal.list.values.each do |s|
  Signal.trap s, 'IGNORE'
end


