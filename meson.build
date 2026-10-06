project(
  'seemusic',
  'cpp',
  version : '0.1',
  meson_version : '>= 1.3.0',
  default_options : ['warning_level=3', 'cpp_std=c++20'],
)

dependencies = [
  dependency('sdl2'),
  dependency('gl'),
  dependency('nlohmann_json'),
  dependency('uuid'),
  dependency('libavformat'),
  dependency('libavcodec'),
  dependency('libswresample'),
  dependency('libavutil'),
]

sources = [
  'backend/Player.cpp',
  'backend/Playlist.cpp',
  'backend/Songs.cpp',
  'Main.cpp',
  'frontend/FrontEndMain.cpp',
  'frontend/RendererGl.cpp',
  'frontend/SeeMusicMain.cpp',
]

exe = executable(
  'seemusic',
  sources,
  install : true,
  include_directories : [ 'backend' ],
  dependencies : dependencies,
)
