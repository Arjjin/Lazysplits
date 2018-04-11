#pragma once

namespace Lazysplits{

enum LZS_PIPE_STATE{ PIPE_ERROR, PIPE_NOT_CREATED, PIPE_CREATED, PIPE_CONNECTION_PENDING, PIPE_CONNECTED, PIPE_BROKEN };
enum LZS_PIPE_TASK_TYPE{ TASK_TYPE_ERROR, TASK_TYPE_CONNECT, TASK_TYPE_READ, TASK_TYPE_WRITE };
enum LZS_PIPE_TASK_STATUS{ TASK_STATUS_ERROR, TASK_STATUS_CREATED, TASK_STATUS_RUNNING, TASK_STATUS_COMPLETED, TASK_STATUS_CANCELLED };

} //namespace Lazysplits