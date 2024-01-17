import { Controller, Inject, UseFilters, UseInterceptors, UsePipes, ValidationPipe } from '@nestjs/common';
import { Ctx, MessagePattern, Payload, RpcException } from '@nestjs/microservices';
import { StatusReportMessage, StatusReportPayload } from '/src/devices/messages/status-report.message';
import { RpcExceptionFilter } from '/libs/filters';
import { DevicesService } from '/src/devices/devices.service';
import { MessageLoggerInterceptor } from '/libs/interceptors/message-logger/message-logger.interceptor';
import { TimeoutInterceptor } from '/libs/interceptors/timeout/timeout.interceptor';
import { MessageIdInterceptor } from '/libs/interceptors/message-id/message-id.interceptor';
import { MqttRequest } from '/libs/interceptors/request.type';
import { FeedService } from '/src/feed/feed.service';
import { ComponentsService } from '/src/components/components.service';
import { DeviceStatus } from '/src/devices/enums/status.enum';
import { validateSync } from 'class-validator';

@UseFilters(RpcExceptionFilter)
@UseInterceptors(MessageIdInterceptor, MessageLoggerInterceptor, TimeoutInterceptor)
@UsePipes(
  new ValidationPipe({
    transform: true,
    whitelist: true,
  }),
)
@Controller('devices')
export class DevicesMqttController {
  constructor(
    @Inject(DevicesService) private readonly devicesService: DevicesService,
    @Inject(FeedService) private readonly feedService: FeedService,
    @Inject(ComponentsService) private readonly componentsService: ComponentsService,
  ) {}

  @MessagePattern('home/devices/+/state')
  async handleReport(@Payload() payload: StatusReportPayload, @Ctx() context: MqttRequest) {
    // TODO:  Save data to database for processing
    const report = StatusReportMessage.fromPayload(payload);
    const error = validateSync(report);
    if (error.length > 0) throw new RpcException(error);

    this.feedService.push(report);

    await this.componentsService.updateManyByDeviceCode(report.code, {
      inUse: report.status === DeviceStatus.ON,
    });

    // Todo Update inUse to false after duration

    await this.devicesService.softCreateDevice({
      code: report.code,
      type: report.type,
      name: report.name,
      version: report.version,
    });
  }
}
