- interface: Task                               任务执行者
  - class: TaskAxiosImpl                        Axios 的具体实现
  - class: TaskWxImpl                           微信小程序 Request 的具体实现
- interface: TaskQueue                          队列模式
  - class: TaskQueueParallelImpl                并行模式具体实现
  - class: TaskQueueSerialImpl                  串行模式具体实现
- interface: RequestSuccessListener             成功回调
- interface: RequestFailListener                失败回调
- interface: RequestCompleteListener            完成回调
- interface: RequestUploadProgressListener      文件上传进度回调
- interface: RequestDownloadProgressListener    文件下载进度回调
- interface: RequestFrontListener               Request 前置回调
- class: RequestConfig                          配置文件
- class: DefaultRequestConfig                   默认配置文件
- class: Request                                请求体
- class: RequestQueue                           请求队列ss