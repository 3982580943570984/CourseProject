project('Course Project', 'cpp')

incdir = include_directories(
  '/home/_3982580943570984/Desktop/imgui',
  '/home/_3982580943570984/Desktop/imgui/backends',
  '/home/_3982580943570984/Desktop/imgui/misc/cpp',
  '/home/_3982580943570984/Desktop/implot',
  '/home/_3982580943570984/Desktop/imnodes'
)

glfw_lib = subproject('glfw').get_variable('glfw_dep')
gl = dependency('gl')

files = files(
  './source/main.cpp',
  './source/Application.cpp',
  './source/BTree.cpp',
  './source/BTreeNode.cpp',

  '/home/_3982580943570984/Desktop/imnodes/ImNodes.cpp',
  '/home/_3982580943570984/Desktop/imnodes/ImNodesEz.cpp',

  '/home/_3982580943570984/Desktop/implot/implot.cpp',
  '/home/_3982580943570984/Desktop/implot/implot_demo.cpp',
  '/home/_3982580943570984/Desktop/implot/implot_items.cpp',

  '/home/_3982580943570984/Desktop/imgui/imgui.cpp',
  '/home/_3982580943570984/Desktop/imgui/imgui_draw.cpp',
  '/home/_3982580943570984/Desktop/imgui/imgui_tables.cpp',
  '/home/_3982580943570984/Desktop/imgui/imgui_demo.cpp',
  '/home/_3982580943570984/Desktop/imgui/imgui_widgets.cpp',
  '/home/_3982580943570984/Desktop/imgui/backends/imgui_impl_glfw.cpp',
  '/home/_3982580943570984/Desktop/imgui/backends/imgui_impl_opengl3.cpp',
  '/home/_3982580943570984/Desktop/imgui/misc/cpp/imgui_stdlib.cpp',
)

executable('BTree Application', files, include_directories: incdir, dependencies: [glfw_lib, gl] )
