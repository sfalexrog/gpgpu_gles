#pragma once

enum GLValues
{
_GL_MAX_TEXTURE_SIZE = 0x0D33,
_GL_MAX_VIEWPORT_DIMS = 0x0D3A,
_GL_MAX_VERTEX_ATTRIBS = 0x8869,
_GL_MAX_VERTEX_UNIFORM_VECTORS = 0x8DFB,
_GL_MAX_VARYING_VECTORS = 0x8DFC,
_GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D,
_GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 0x8B4C,
_GL_MAX_TEXTURE_IMAGE_UNITS = 0x8872,
_GL_MAX_FRAGMENT_UNIFORM_VECTORS = 0x8DFD,
_GL_MAX_CUBE_MAP_TEXTURE_SIZE = 0x851C,
_GL_MAX_RENDERBUFFER_SIZE = 0x84E8,
_GL_MAX_3D_TEXTURE_SIZE = 0x8073,
_GL_MAX_ELEMENTS_VERTICES = 0x80E8,
_GL_MAX_ELEMENTS_INDICES = 0x80E9,
_GL_MAX_TEXTURE_LOD_BIAS = 0x84FD,
_GL_MAX_DRAW_BUFFERS = 0x8824,
_GL_MAX_FRAGMENT_UNIFORM_COMPONENTS = 0x8B49,
_GL_MAX_VERTEX_UNIFORM_COMPONENTS = 0x8B4A,
_GL_MAX_ARRAY_TEXTURE_LAYERS = 0x88FF,
_GL_MAX_PROGRAM_TEXEL_OFFSET = 0x8905,
_GL_MAX_VARYING_COMPONENTS = 0x8B4B,
_GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS = 0x8C80,
_GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS = 0x8C8A,
_GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS = 0x8C8B,
_GL_MAX_COLOR_ATTACHMENTS = 0x8CDF,
_GL_MAX_SAMPLES = 0x8D57,
_GL_MAX_VERTEX_UNIFORM_BLOCKS = 0x8A2B,
_GL_MAX_FRAGMENT_UNIFORM_BLOCKS = 0x8A2D,
_GL_MAX_COMBINED_UNIFORM_BLOCKS = 0x8A2E,
_GL_MAX_UNIFORM_BUFFER_BINDINGS = 0x8A2F,
_GL_MAX_UNIFORM_BLOCK_SIZE = 0x8A30,
_GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS = 0x8A31,
_GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS = 0x8A33,
_GL_MAX_VERTEX_OUTPUT_COMPONENTS = 0x9122,
_GL_MAX_FRAGMENT_INPUT_COMPONENTS = 0x9125,
_GL_MAX_SERVER_WAIT_TIMEOUT = 0x9111,
_GL_MAX_ELEMENT_INDEX = 0x8D6B,
_GL_MAX_COMPUTE_UNIFORM_BLOCKS = 0x91BB,
_GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS = 0x91BC,
_GL_MAX_COMPUTE_IMAGE_UNIFORMS = 0x91BD,
_GL_MAX_COMPUTE_SHARED_MEMORY_SIZE = 0x8262,
_GL_MAX_COMPUTE_UNIFORM_COMPONENTS = 0x8263,
_GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS = 0x8264,
_GL_MAX_COMPUTE_ATOMIC_COUNTERS = 0x8265,
_GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS = 0x8266,
_GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS = 0x90EB,
_GL_MAX_COMPUTE_WORK_GROUP_COUNT = 0x91BE,
_GL_MAX_COMPUTE_WORK_GROUP_SIZE = 0x91BF,
_GL_MAX_UNIFORM_LOCATIONS = 0x826E,
_GL_MAX_FRAMEBUFFER_WIDTH = 0x9315,
_GL_MAX_FRAMEBUFFER_HEIGHT = 0x9316,
_GL_MAX_FRAMEBUFFER_SAMPLES = 0x9318,
_GL_MAX_NAME_LENGTH = 0x92F6,
_GL_MAX_NUM_ACTIVE_VARIABLES = 0x92F7,
_GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS = 0x92CC,
_GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS = 0x92D0,
_GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS = 0x92D1,
_GL_MAX_VERTEX_ATOMIC_COUNTERS = 0x92D2,
_GL_MAX_FRAGMENT_ATOMIC_COUNTERS = 0x92D6,
_GL_MAX_COMBINED_ATOMIC_COUNTERS = 0x92D7,
_GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE = 0x92D8,
_GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS = 0x92DC,
_GL_MAX_IMAGE_UNITS = 0x8F38,
_GL_MAX_VERTEX_IMAGE_UNIFORMS = 0x90CA,
_GL_MAX_FRAGMENT_IMAGE_UNIFORMS = 0x90CE,
_GL_MAX_COMBINED_IMAGE_UNIFORMS = 0x90CF,
_GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS = 0x90D6,
_GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS = 0x90DA,
_GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS = 0x90DB,
_GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS = 0x90DC,
_GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS = 0x90DD,
_GL_MAX_SHADER_STORAGE_BLOCK_SIZE = 0x90DE,
_GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES = 0x8F39,
_GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET = 0x8E5F,
_GL_MAX_SAMPLE_MASK_WORDS = 0x8E59,
_GL_MAX_COLOR_TEXTURE_SAMPLES = 0x910E,
_GL_MAX_DEPTH_TEXTURE_SAMPLES = 0x910F,
_GL_MAX_INTEGER_SAMPLES = 0x9110,
_GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET = 0x82D9,
_GL_MAX_VERTEX_ATTRIB_BINDINGS = 0x82DA,
_GL_MAX_VERTEX_ATTRIB_STRIDE = 0x82E5,
_GL_MAX_COLOR_FRAMEBUFFER_SAMPLES_AMD = 0x91B3,
_GL_MAX_COLOR_FRAMEBUFFER_STORAGE_SAMPLES_AMD = 0x91B4,
_GL_MAX_DEPTH_STENCIL_FRAMEBUFFER_SAMPLES_AMD = 0x91B5,
_GL_MAX_SAMPLES_ANGLE = 0x8D57,
_GL_MAX_CLIP_DISTANCES_APPLE = 0x0D32,
_GL_MAX_SAMPLES_APPLE = 0x8D57,
_GL_MAX_SERVER_WAIT_TIMEOUT_APPLE = 0x9111,
_GL_MAX_DUAL_SOURCE_DRAW_BUFFERS_EXT = 0x88FC,
_GL_MAX_EXT = 0x8008,
_GL_MAX_CLIP_DISTANCES_EXT = 0x0D32,
_GL_MAX_CULL_DISTANCES_EXT = 0x82F9,
_GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES_EXT = 0x82FA,
_GL_MAX_COLOR_ATTACHMENTS_EXT = 0x8CDF,
_GL_MAX_DRAW_BUFFERS_EXT = 0x8824,
_GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_EXT = 0x8DDF,
_GL_MAX_GEOMETRY_UNIFORM_BLOCKS_EXT = 0x8A2C,
_GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS_EXT = 0x8A32,
_GL_MAX_GEOMETRY_INPUT_COMPONENTS_EXT = 0x9123,
_GL_MAX_GEOMETRY_OUTPUT_COMPONENTS_EXT = 0x9124,
_GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT = 0x8DE0,
_GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_EXT = 0x8DE1,
_GL_MAX_GEOMETRY_SHADER_INVOCATIONS_EXT = 0x8E5A,
_GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_EXT = 0x8C29,
_GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS_EXT = 0x92CF,
_GL_MAX_GEOMETRY_ATOMIC_COUNTERS_EXT = 0x92D5,
_GL_MAX_GEOMETRY_IMAGE_UNIFORMS_EXT = 0x90CD,
_GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS_EXT = 0x90D7,
_GL_MAX_FRAMEBUFFER_LAYERS_EXT = 0x9317,
_GL_MAX_SAMPLES_EXT = 0x8D57,
_GL_MAX_MULTIVIEW_BUFFERS_EXT = 0x90F2,
_GL_MAX_RASTER_SAMPLES_EXT = 0x9329,
_GL_MAX_SHADER_PIXEL_LOCAL_STORAGE_FAST_SIZE_EXT = 0x8F63,
_GL_MAX_SHADER_PIXEL_LOCAL_STORAGE_SIZE_EXT = 0x8F67,
_GL_MAX_SHADER_COMBINED_LOCAL_STORAGE_FAST_SIZE_EXT = 0x9650,
_GL_MAX_SHADER_COMBINED_LOCAL_STORAGE_SIZE_EXT = 0x9651,
_GL_MAX_SPARSE_TEXTURE_SIZE_EXT = 0x9198,
_GL_MAX_SPARSE_3D_TEXTURE_SIZE_EXT = 0x9199,
_GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS_EXT = 0x919A,
_GL_MAX_PATCH_VERTICES_EXT = 0x8E7D,
_GL_MAX_TESS_GEN_LEVEL_EXT = 0x8E7E,
_GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS_EXT = 0x8E7F,
_GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS_EXT = 0x8E80,
_GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS_EXT = 0x8E81,
_GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS_EXT = 0x8E82,
_GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS_EXT = 0x8E83,
_GL_MAX_TESS_PATCH_COMPONENTS_EXT = 0x8E84,
_GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS_EXT = 0x8E85,
_GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS_EXT = 0x8E86,
_GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS_EXT = 0x8E89,
_GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS_EXT = 0x8E8A,
_GL_MAX_TESS_CONTROL_INPUT_COMPONENTS_EXT = 0x886C,
_GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS_EXT = 0x886D,
_GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS_EXT = 0x8E1E,
_GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS_EXT = 0x8E1F,
_GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS_EXT = 0x92CD,
_GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS_EXT = 0x92CE,
_GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS_EXT = 0x92D3,
_GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS_EXT = 0x92D4,
_GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS_EXT = 0x90CB,
_GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS_EXT = 0x90CC,
_GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS_EXT = 0x90D8,
_GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS_EXT = 0x90D9,
_GL_MAX_TEXTURE_BUFFER_SIZE_EXT = 0x8C2B,
_GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = 0x84FF,
_GL_MAX_WINDOW_RECTANGLES_EXT = 0x8F14,
_GL_MAX_SAMPLES_IMG = 0x9135,
_GL_MAX_DEBUG_GROUP_STACK_DEPTH = 0x826C,
_GL_MAX_LABEL_LENGTH = 0x82E8,
_GL_MAX_DEBUG_MESSAGE_LENGTH = 0x9143,
_GL_MAX_DEBUG_LOGGED_MESSAGES = 0x9144,
_GL_MAX_DEBUG_GROUP_STACK_DEPTH_KHR = 0x826C,
_GL_MAX_LABEL_LENGTH_KHR = 0x82E8,
_GL_MAX_DEBUG_MESSAGE_LENGTH_KHR = 0x9143,
_GL_MAX_DEBUG_LOGGED_MESSAGES_KHR = 0x9144,
_GL_MAX_SHADER_COMPILER_THREADS_KHR = 0x91B0,
_GL_MAX_SUBPIXEL_PRECISION_BIAS_BITS_NV = 0x9349,
_GL_MAX_DRAW_BUFFERS_NV = 0x8824,
_GL_MAX_COLOR_ATTACHMENTS_NV = 0x8CDF,
_GL_MAX_SAMPLES_NV = 0x8D57,
_GL_MAX_DETACHED_TEXTURES_NV = 0x95AC,
_GL_MAX_DETACHED_BUFFERS_NV = 0x95AD,
_GL_MAX_MESH_UNIFORM_BLOCKS_NV = 0x8E60,
_GL_MAX_MESH_TEXTURE_IMAGE_UNITS_NV = 0x8E61,
_GL_MAX_MESH_IMAGE_UNIFORMS_NV = 0x8E62,
_GL_MAX_MESH_UNIFORM_COMPONENTS_NV = 0x8E63,
_GL_MAX_MESH_ATOMIC_COUNTER_BUFFERS_NV = 0x8E64,
_GL_MAX_MESH_ATOMIC_COUNTERS_NV = 0x8E65,
_GL_MAX_MESH_SHADER_STORAGE_BLOCKS_NV = 0x8E66,
_GL_MAX_COMBINED_MESH_UNIFORM_COMPONENTS_NV = 0x8E67,
_GL_MAX_TASK_UNIFORM_BLOCKS_NV = 0x8E68,
_GL_MAX_TASK_TEXTURE_IMAGE_UNITS_NV = 0x8E69,
_GL_MAX_TASK_IMAGE_UNIFORMS_NV = 0x8E6A,
_GL_MAX_TASK_UNIFORM_COMPONENTS_NV = 0x8E6B,
_GL_MAX_TASK_ATOMIC_COUNTER_BUFFERS_NV = 0x8E6C,
_GL_MAX_TASK_ATOMIC_COUNTERS_NV = 0x8E6D,
_GL_MAX_TASK_SHADER_STORAGE_BLOCKS_NV = 0x8E6E,
_GL_MAX_COMBINED_TASK_UNIFORM_COMPONENTS_NV = 0x8E6F,
_GL_MAX_MESH_WORK_GROUP_INVOCATIONS_NV = 0x95A2,
_GL_MAX_TASK_WORK_GROUP_INVOCATIONS_NV = 0x95A3,
_GL_MAX_MESH_TOTAL_MEMORY_SIZE_NV = 0x9536,
_GL_MAX_TASK_TOTAL_MEMORY_SIZE_NV = 0x9537,
_GL_MAX_MESH_OUTPUT_VERTICES_NV = 0x9538,
_GL_MAX_MESH_OUTPUT_PRIMITIVES_NV = 0x9539,
_GL_MAX_TASK_OUTPUT_COUNT_NV = 0x953A,
_GL_MAX_DRAW_MESH_TASKS_COUNT_NV = 0x953D,
_GL_MAX_MESH_VIEWS_NV = 0x9557,
_GL_MAX_MESH_WORK_GROUP_SIZE_NV = 0x953B,
_GL_MAX_TASK_WORK_GROUP_SIZE_NV = 0x953C,
_GL_MAX_COARSE_FRAGMENT_SAMPLES_NV = 0x955F,
_GL_MAX_VIEWPORTS_NV = 0x825B,
_GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_OES = 0x8DDF,
_GL_MAX_GEOMETRY_UNIFORM_BLOCKS_OES = 0x8A2C,
_GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS_OES = 0x8A32,
_GL_MAX_GEOMETRY_INPUT_COMPONENTS_OES = 0x9123,
_GL_MAX_GEOMETRY_OUTPUT_COMPONENTS_OES = 0x9124,
_GL_MAX_GEOMETRY_OUTPUT_VERTICES_OES = 0x8DE0,
_GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_OES = 0x8DE1,
_GL_MAX_GEOMETRY_SHADER_INVOCATIONS_OES = 0x8E5A,
_GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_OES = 0x8C29,
_GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS_OES = 0x92CF,
_GL_MAX_GEOMETRY_ATOMIC_COUNTERS_OES = 0x92D5,
_GL_MAX_GEOMETRY_IMAGE_UNIFORMS_OES = 0x90CD,
_GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS_OES = 0x90D7,
_GL_MAX_FRAMEBUFFER_LAYERS_OES = 0x9317,
_GL_MAX_FRAGMENT_INTERPOLATION_OFFSET_OES = 0x8E5C,
_GL_MAX_PATCH_VERTICES_OES = 0x8E7D,
_GL_MAX_TESS_GEN_LEVEL_OES = 0x8E7E,
_GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS_OES = 0x8E7F,
_GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS_OES = 0x8E80,
_GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS_OES = 0x8E81,
_GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS_OES = 0x8E82,
_GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS_OES = 0x8E83,
_GL_MAX_TESS_PATCH_COMPONENTS_OES = 0x8E84,
_GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS_OES = 0x8E85,
_GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS_OES = 0x8E86,
_GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS_OES = 0x8E89,
_GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS_OES = 0x8E8A,
_GL_MAX_TESS_CONTROL_INPUT_COMPONENTS_OES = 0x886C,
_GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS_OES = 0x886D,
_GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS_OES = 0x8E1E,
_GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS_OES = 0x8E1F,
_GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS_OES = 0x92CD,
_GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS_OES = 0x92CE,
_GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS_OES = 0x92D3,
_GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS_OES = 0x92D4,
_GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS_OES = 0x90CB,
_GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS_OES = 0x90CC,
_GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS_OES = 0x90D8,
_GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS_OES = 0x90D9,
_GL_MAX_3D_TEXTURE_SIZE_OES = 0x8073,
_GL_MAX_TEXTURE_BUFFER_SIZE_OES = 0x8C2B,
_GL_MAX_VIEWPORTS_OES = 0x825B,
_GL_MAX_VIEWS_OVR = 0x9631,
_GL_MAX_SHADER_SUBSAMPLED_IMAGE_UNITS_QCOM = 0x8FA1,
};