#!/bin/env python3
# -*- coding: UTF-8 -*-

"""
Copyright (c) 2015 sensorsdata.cn, Inc. All Rights Reserved
@author padme(jinsilan@sensorsdata.cn)
@brief

支持各种导入
"""
import abc
import argparse
import csv
import datetime
import decimal
import hashlib
import json
import logging
import logging.handlers
import os
import pprint
import random
import re
import sys
import time
import traceback

try:
    import urllib.parse as urllib
    import urllib.request as urllib2
except ImportError:
    import urllib2
    import urllib

__version__ = '1.6.1'

# build的时候会把python sdk和pypinyin,pymysql都拷贝过来
current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(current_dir)
import pymysql
import pypinyin
import sensorsanalytics

logger_name = 'format_importer'
log_file = '%s/format_importer.log' % current_dir
# 配置logger整体
logger = logging.getLogger(logger_name)
logger.setLevel(logging.DEBUG)
formater = logging.Formatter('%(asctime)s %(lineno)d %(levelname)s %(message)s')
# 配置console 打印INFO级别
console = logging.StreamHandler()
console.setLevel(logging.INFO)
console.setFormatter(formater)
logger.addHandler(console)
# 单个文件最大1m 最多10个文件
fa = logging.handlers.RotatingFileHandler(log_file, 'a', 1024 * 1024, 10)
fa.setLevel(logging.DEBUG)
fa.setFormatter(formater)
logger.addHandler(fa)

class SAArgumentParser(argparse.ArgumentParser):
    '''支持从文件读取 文件可以包含注释空行'''
    def convert_arg_line_to_args(self, arg_line):
        strip_line = arg_line.strip()
        # 空行
        if not strip_line:
            return []
        # 注释
        if strip_line.startswith('#'):
            return []
        fields = strip_line.split(':')
        # 只有一个参数 比如--debug
        if len(fields) == 1:
            return ['--%s' % fields[0]]
        # 两个参数，注意合理的strip()
        first = fields[0].strip()
        second = ':'.join(fields[1:]).strip()
        return ['--%s' % first, second]

####
#
# 通用方法
#
###
def isnumber(target_str):
    try:
        float(target_str)
        # 能成功转换为浮点型，则是数字
        return True
    except:
        # 不能成功转换为浮点型，则不是数字
        return False

def isbool(target_str):
    try:
        return target_str.lower() in ['true', 'false']
    except:
        # 不能成功转换为浮点型，则不是数字
        return False

ename_pattern = re.compile(r'[\$]{0,1}[a-zA-Z0-9_]+$')
def get_ename(cname):
    '''
    从中文转拼音
    '''
    if ename_pattern.match(cname):
        return cname
    first_ename = pypinyin.slug(cname, separator='', errors='default')
    first_ename = first_ename.lower()
    second_ename = ''
    for ch in first_ename:
        if ch in 'abcdefghijklmnopqrstuvwxyz1234567890':
            second_ename += ch
    second_ename = second_ename[0:90]
    return second_ename

def send_request(url, content=None, headers={}):
    '''
    发送get请求(args同urllib2.Request)
    '''
    for i in range(3):
        try:
            if i != 0:
                time.sleep(2)
            request_content = json.dumps(content).encode() if content else None
            request = urllib2.Request(url=url, data=request_content, headers=headers)
            response = urllib2.urlopen(request)
            response_content = json.loads(response.read().decode('utf-8'))
            logger.debug('request %s succeed' % url)
            logger.debug('content:\n%s\nheaders\n%s\nresponse\n%s' % (content, headers, response_content))
            return response_content
        except Exception:
            logger.warn('failed to request %s for %d times' % (url, i))
            logger.debug(traceback.format_exc())
    else:
        raise Exception('failed to request %s!' % url)

def compile_log_format(log_format_str):
    '''将Nginx log format转化为正则'''
    variable_pattern = re.compile(r'\$[a-zA-Z0-9_]*')
    columns_with_dolla = variable_pattern.findall(log_format_str)
    columns = [x[1:] for x in columns_with_dolla]
    log_format_pattern = log_format_str
    # 1. metachars
    metachars = '\.^*+?{}[]|()'
    for char in metachars:
        log_format_pattern = log_format_pattern.replace(char, '\\%s' % char)
    # 2. 替换$xxx
    for s in columns_with_dolla:
        log_format_pattern = log_format_pattern.replace(s, '(.*)', 1)
    # 3. 整行匹配
    log_format_pattern = '%s$' % log_format_pattern
    logger.info('log_format: %s' % log_format_str)
    logger.info('re pattern: %s' % log_format_pattern)
    logger.info('columns: %s' % columns)
    return columns, re.compile(r'%s' % log_format_pattern)

def parse_url(url):
    """解析url"""
    parse_result = urllib.urlparse(urllib.unquote(url))
    ret = {
        'netloc': parse_result.netloc,
        'path': parse_result.path,
        'query': parse_result.query,
    }
    params = urllib.parse_qs(parse_result.query)
    for k, v in params.items():
        ret['param_%s' % k] = v[0]
    return ret

def parse_request(request):
    """解析request:GET /behavior/access/?p=click&uid="""
    fields = request.split(' ')
    ret = {'__request_method': fields[0]}
    rest = ' '.join(fields[1:])
    parse_result = urllib.urlparse(rest)
    ret['__request_path'] = parse_result.path
    ret['__request_query'] = parse_result.query
    params = urllib.parse_qs(parse_result.query)
    for k, v in params.items():
        ret['__request_param_%s' % k] = v[0]
    return ret

def parse_getter(args):
    """ 定制获取event, distinct_id, timestamp的方法 """
    # 1. distinct_id_getter
    def distinct_id_getter(record):
        if args.distinct_id_from not in record:
            raise Exception('cannot find distinct_id[%s] in record[%s]' % (args.distinct_id_from, record))
        return record[args.distinct_id_from]
    if args.subparser_name.endswith('_profile'):
        return None, distinct_id_getter, None
    # 2. event_getter
    if args.event_from:
        def event_getter(record):
            if args.event_from not in record:
                raise Exception('cannot find event[%s] in record[%s]' % (args.event_from, record))
            return record[args.event_from]
    else:
        event_getter = lambda record: args.event_default
    # 3. timestamp_getter
    if args.timestamp_from:
        def timestamp_getter(record):
            if args.timestamp_from not in record:
                raise Exception('cannot find timestamp[%s] in record[%s]' % (args.timestamp_from, record))
            elif type(record[args.timestamp_from]) is str:
                return datetime.datetime.strptime(record[args.timestamp_from], args.timestamp_format)
            else:
                return record[args.timestamp_from]
    elif args.timestamp_default:
        timestamp_getter = lambda record: datetime.datetime.strptime(args.timestamp_default, args.timestamp_format)
    else:
        timestamp_getter = lambda record: datetime.datetime.now()
    return event_getter, distinct_id_getter, timestamp_getter

def parse_args():
    # 1. 定义parser
    parent_parser = SAArgumentParser(add_help=False)
    # 公用参数
    # 1.1. url, username, password
    parent_parser.add_argument('--url', '-l',
            help='必填,发送数据的url，比如http://localhost:8006/sa, ' \
                    '如果是云版则类似http://abc.cloud.sensorsdata.cn:8006/sa?token=xxx, ' \
                    'token请联系我们获取',
            required=True)
    parent_parser.add_argument('--project', '-j',
            help='可选，指定的project名，默认是None',
            default=None,
            required=False)
    # 1.2 指定distinct_id
    # distinct_id
    parent_parser.add_argument('--distinct_id_from', '-df',
            help='必填, 从哪个字段作为distinct_id，如果指定，则每条数据算作对应字段的用户的行为.',
            required=True)
    # 1.3 断点续传
    parent_parser.add_argument('--skip_cnt', '-c',
            type=int,
            help='第一次运行请忽略，如果运行失败，需要跳过成功的那集行，这个就是指定跳过几行的',
            default=0)
    parent_parser.add_argument('--debug', '-D',
            action='store_true',
            help='如果指定了就是使用debug模式，不会导入数据，只在stdout显示数据，参见(https://www.sensorsdata.cn/manual/debug_mode.html)',
            default=False)
    parent_parser.add_argument('--quit_on_error', '-Q',
            action='store_true',
            help='如果选中，则出现一条错误日志就会退出',
            default=False)

    # 2. profile/event有不同的选项
    profile_parent_parser = parent_parser
    event_parent_parser = SAArgumentParser(add_help=False, parents=[parent_parser])
    # event必须要么指定event_from要么指定event_default
    event_group = event_parent_parser.add_mutually_exclusive_group(required=True)
    event_group.add_argument('--event_from', '-ef',
            help='和event_default选一个必填。哪个字段作为event名，如果指定，则每条数据的事件名为对应字段的值。')
    event_group.add_argument('--event_default', '-ed',
            help='和event_from选一个必填。默认的event名，如果指定，则将所有数据都算作这个event的。')
    # timestammp必须要么指定timestamp_from 要么指定timestamp_default 要么都不指定
    timestamp_group = event_parent_parser.add_mutually_exclusive_group(required=False)
    timestamp_group.add_argument('--timestamp_from', '-tf',
            help='哪个字段作为time, 如果指定，则每条数据对应的时间为对应字段的值.')
    timestamp_group.add_argument('--timestamp_default', '-td',
            help='默认的timestamp, 如果指定，则将所有数据都算作这个时间的事件。')
    event_parent_parser.add_argument('--timestamp_format', '-fm',
            help='和timestamp_from一起使用，如果指定, 并timestamp_from对应的字段是个字符串，可以通过这种方式指定时间格式。' \
                    '默认是%%Y-%%m-%%d %%H:%%M:%%S',
            default='%Y-%m-%d %H:%M:%S')
    # 2. 支持三中格式
    parser = SAArgumentParser()
    subparsers = parser.add_subparsers(help='通用格式文件导入工具', dest='subparser_name')
    for format_name in ['csv', 'mysql', 'nginx']:
        formatter_name = '%sFormatter' % format_name.title()
        formatter_class = globals()[formatter_name]
        sub_parser = subparsers.add_parser(
            '%s_profile' % format_name,
            parents=[profile_parent_parser],
            help='%s, 导入profile' % formatter_class.__doc__.splitlines()[0],
            formatter_class=argparse.RawDescriptionHelpFormatter,
            description=formatter_class.__doc__,
            fromfile_prefix_chars='@')
        formatter_class.add_parser(sub_parser)
        sub_parser = subparsers.add_parser(
            '%s_event' % format_name,
            parents=[event_parent_parser],
            help='%s, 导入event' % formatter_class.__doc__.splitlines()[0],
            formatter_class=argparse.RawDescriptionHelpFormatter,
            description=formatter_class.__doc__,
            fromfile_prefix_chars='@')
        formatter_class.add_parser(sub_parser)
    # 3. 解析
    args = parser.parse_args()
    if not args.subparser_name:
        parser.print_help()
        sys.exit(1)
    return args

######
#
# 从这里开始各个formater的实现
# 
######
class BaseFormatter(object):
    def __init__(self, args, lib_detail):
        '''初始化方法'''
        self.is_event = args.subparser_name.endswith('_event')
        self.get_event, self.get_distinct_id, self.get_timestamp = parse_getter(args)
        self.send = self.send_event if self.is_event else self.send_profile
        # 过滤掉用于获取event/time/user的列
        self.skip_cols = [args.distinct_id_from]
        if self.is_event:
            if args.event_from:
                self.skip_cols.append(args.event_from)
            if args.timestamp_from:
                self.skip_cols.append(args.timestamp_from)
        # 记录导入信息的
        lib_detail_title = {'mysql': 'MySQL', 'csv': 'csv', 'nginx': 'Nginx'}[args.subparser_name.split('_')[0]]
        self.default_properties = {
            '$lib': 'FormatImporter',
            '$lib_version': __version__,
            '$lib_method': 'tools',
            '$lib_detail': ('%s##%s' % (lib_detail_title, lib_detail))[:100],
        }

    @staticmethod
    @abc.abstractmethod
    def add_parser(sub_parser):
        '''初始化sub_parser'''
        pass

    @abc.abstractmethod
    def get_total_num(self):
        '''返回总条数'''
        pass

    @abc.abstractmethod
    def read_records(self):
        '''返回一行'''
        pass

    @abc.abstractmethod
    def parse_property(self, record):
        '''解析properties'''
        pass

    @abc.abstractmethod
    def send_event(self, sa, record):
        '''发送event'''
        event = self.get_event(record)
        distinct_id = self.get_distinct_id(record)
        timestamp = self.get_timestamp(record)
        properties = self.parse_property(record)
        properties['$time'] = timestamp
        sa.track(distinct_id, event, properties)

    @abc.abstractmethod
    def send_profile(self, sa, record):
        '''发送profile'''
        distinct_id = self.get_distinct_id(record)
        properties = self.parse_property(record)
        sa.profile_set(distinct_id, properties)

    @abc.abstractmethod
    def close(self):
        '''清理方法'''
        pass

class CsvFormatter(BaseFormatter):
    """将csv格式文件导入
使用举例:
    1. format_importer.py csv_event -l 'http://localhost:8006/sa' -ef event -df user -td '2014-08-01 12:00:00'
       -u admin -p passwd -f ./test.csv -w 'http://localhost:8007'
       将test.csv中所有列作为properties导入本地的私有部署版本，其中每行是一个事件，event对应的列为事件名，
       user对应的列为distinct_id, 时间全部为2014-08-01 12:00:00
    2. format_importer.py csv_event -l 'http://localhost:8006/sa' -ed my_event -df user -tf time -fm '%Y%m%d%H%M%S'
       -u admin -p passwd -f ./test.csv -w 'http://localhost:8007' -pl col1,col2,col3
       将test.csv中(col1, col2, col3)作为properties导入本地的私有部署版本，对应事件名全部为my_event,
       每行是一个事件，user对应的列为distinct_id, time对应的列为time(格式类似20150130125959)
    3. format_importer.py csv_profile -l 'http://localhost:8006/sa' -df user -u admin -p passwd -f
       ./test.csv -w 'http://localhost:8007'
       将test.csv中所有的列作为用户属性导入本地的私有部署版本，其中没一行是一个用户，user对应的列为distinct_id
    4. format_importer.py csv_event @./conf/csv_event.conf
       也可以修改conf/csv_event.conf和conf/csv_profile.conf来填写所有必要参数
    """
    def __init__(self, args):
        '''初始化方法'''
        super().__init__(args, args.filename)
        # 1. 参数校验
        skip_identify_list = args.skip_identify.split(',') if args.skip_identify else []
        property_list = args.property_list.split(',') if args.property_list else []
        self.ignore_value = args.ignore_value
        self.ignore_value.append('')
        logger.debug('ignore %s' % self.ignore_value)
        # 2. 记下url username password close()的时候要更新元数据
        self.add_cname = False
        if args.subparser_name.endswith('_event'):
            if args.add_cname:
                self.add_cname = True
                self.web_url, self.username, self.password = args.web_url, args.username, args.password
                querys = urllib.parse_qs(urllib.urlparse(args.url).query)
                self.project = querys.get('project', ['default'])[0]
        # 3. 初始化csv reader
        self.fd = open(args.filename, 'r')
        self.reader = csv.DictReader(self.fd, delimiter=args.csv_delimiter, quotechar=args.csv_quotechar)
        self.events = set()
        # 4. 获取schema 中间会用fd和reader 所以使用完要reset
        self.column_schema = self.__get_column_schema(skip_identify_list, property_list, args.csv_prefetch_lines)
        self.fd.close()
        self.fd = open(args.filename, 'r')
        self.reader = csv.DictReader(self.fd, delimiter=args.csv_delimiter, quotechar=args.csv_quotechar)

        logger.info('column_schema: %s' % pprint.pformat(self.column_schema, width=200))
        logger.info('events: %s' % self.events)
        logger.info('start import csv from %s' % args.filename)

    @staticmethod
    def add_parser(parser):
        '''初始化sub_parser'''
        parser.add_argument('--filename', '-f',
                help='必填,csv文件名',
                required=True)
        parser.add_argument('--property_list', '-pl',
                type=str,
                help='用逗号分割选取的property, ' \
                        '举例`-p name,time`将会将name和time两列作为property导入。' \
                        '如果不填写则表示全部作为property导入',
                default=None)
        parser.add_argument('--skip_identify', '-i',
                help='对应的列将不会做自动类型判断，' \
                        '举例`--skip_identify name,id`将会将name和id不做类型判断，完全作为string导入' \
                        '如果不填写则表示全部的选中列都会自动做类型判断')
        parser.add_argument('--add_cname', '-ac',
                action='store_true',
                help='是否添加中文名，只对event有效. 如果csv的表头是中文，程序会将对应的property名' \
                        '改为对应的拼音。这时，如果将add_cname选中，会自动再程序的最后把中英文的映' \
                        '射关系填上去，这样在Ui上看到的对应property就是中文的了.',
                default=False)
        parser.add_argument('--web_url', '-w',
                help='如果选择add_cname则必填，web访问的url，单机版类似http://localhost:8007, cloud' \
                        '版类似http://xxx.cloud.sensorsdata.cn')
        parser.add_argument('--username', '-u',
                help='如果选择add_cname则必填, web登录用户名')
        parser.add_argument('--password', '-p',
                help='如果选择add_cname则必填, web登录密码')
        parser.add_argument('--ignore_value',
                action='append',
                default=[],
                help='指定某些值为空，比如指定 `--ignore_value null` 则所有的null都被认为是空值')
        parser.add_argument('--csv_delimiter',
                type=str,
                help='csv文件的列分隔符，默认为\',\'，只接受单字符参数',
                default=',')
        parser.add_argument('--csv_quotechar',
                type=str,
                help='csv文件的引用字符，用于指定csv字符串的开始和结尾，默认为\'"\'，只接受单字符参数',
                default='"')
        parser.add_argument('--csv_prefetch_lines',
                type=int,
                help='csv文件预读行数，预读用于判断列的类型，默认为-1，即预读整个文件',
                default='-1')

    def get_total_num(self):
        '''返回总条数'''
        return self.total_num

    def read_records(self):
        if self.add_cname:
            cname_to_ename = dict([(x['cname'], x['ename']) for x in self.column_schema.values()])
            for record in self.reader:
                new_record = {cname_to_ename[k]: v for k,v in record.items()}
                yield new_record
        else:
            for record in self.reader:
                yield record

    def parse_property(self, record):
        properties = {}
        for k in self.column_schema:
            if k not in record:
                continue
            if record[k] in self.ignore_value:
                continue
            if self.column_schema[k]['type'] == 'number':
                properties[self.column_schema[k]['ename']] = float(record[k])
            elif self.column_schema[k]['type'] == 'bool':
                properties[self.column_schema[k]['ename']] = (record[k].lower() == 'true')
            else:
                properties[self.column_schema[k]['ename']] = record[k]
        return properties

    def close(self):
        '''清理方法'''
        if self.add_cname:
            self.__update_meta()
        self.fd.close()
        logger.info('end import csv')

    def __update_meta(self):
        '''更新英文名到中文名的对应关系'''
        # 1.获取token
        auth_response = send_request( 
            url='%s/api/auth/login?project=%s' % (self.web_url, self.project),
            content={'username': self.username, 'password': self.password},
            headers={'Content-Type': 'application/json'})
        auth_token = auth_response['token']

        # 2.取所有events，得到对应事件event_id
        for i in range(10):
            if i != 0:
                time.sleep(30)
            events_response = send_request(
                url='%s/api/events/all?cache=false&project=%s' % (self.web_url, self.project),
                headers={'sensorsdata-token': auth_token})
            # print(events_response_json)
            event_dict = {x['name']: x['id'] for x in events_response}
            event_not_exists = [event for event in self.events if event not in event_dict]
            if not event_not_exists:
                break
            logger.warn('events%s have not loaded yet. waiting for 0.5 minute.' % event_not_exists)

        # 3.取这个event所有properties，得到各property的id
        for i in range(10):
            all_checked = True
            if i != 0:
                time.sleep(30)
            event_property_id = {}
            for event in self.events:
                event_id = event_dict[event]
                properties_response = send_request(
                        url='%s/api/event/%s/properties?cache=false&project=%s' \
                                % (self.web_url, event_id, self.project),
                        headers={'sensorsdata-token': auth_token})
                properties_dict = {x['name']: x['id'] for x in properties_response['event']}
                all_properties = [x['ename'] for x in self.column_schema.values()]
                property_not_exists = [x for x in all_properties if x not in properties_dict]
                if property_not_exists:
                    logger.warn('in event %s, properties%s have not loaded yet. waiting for 0.5 minute.' %
                            (event, property_not_exists))
                    all_checked = False
                    break
                event_property_id[event] = properties_dict
            if all_checked:
                break

        # 4.更新元数据
        for event, properties_dict in event_property_id.items():
            event_id = event_dict[event]
            update_meta_request_content = {'event_id': event_id, 'property': []}
            for column in self.column_schema.values():
                update_meta_request_content['property'].append({
                        'property_id': properties_dict[column['ename']],
                        'cname': column['cname']})
            update_meta_response = send_request(
                    url='%s/api/event/%s/meta?project=%s' % (self.web_url, event_id, self.project),
                    content=update_meta_request_content,
                    headers={'sensorsdata-token': auth_token,'Content-Type': 'application/json'})
            logger.info('successfully update cname for event %s.' % event)

    def __get_column_schema(self, skip_identify_list, property_list, prefetch_lines):
        # 1. 遍历一遍，得到每一个列的类型，暂时只需要判断是数字还是字符串
        column_type = {}
        self.total_num = 0
        try:
            for record in self.reader:
                # 头的列数比内容少的话 会多个key为None value为一个list的kv
                if None in record:
                    raise Exception('csv error near line %d: content has %d more fields than header' \
                            % (self.reader.line_num, len(record[None])))
                if self.is_event:
                    event = self.get_event(record)
                    if event not in self.events:
                        self.events.add(event)
                
                self.total_num += 1
                if prefetch_lines > 0 and self.total_num > prefetch_lines:
                    break
                
                # property_list如果用户没有指定 则认为全部导入
                if not property_list:
                    property_list = record.keys()
                for k in property_list:
                    # 将用于选择event/distinc_id/time的列去掉
                    if k in self.skip_cols:
                        continue
                    # 用户指定不做自动检查的列
                    if k in skip_identify_list:
                        column_type[k] = 'string'
                        continue
                    column = record[k]
                    # 内容的列数比头少的话 对应key的value为none
                    if column is None:
                        raise Exception('csv error near line %d: no value for field %s' \
                                % (self.reader.line_num, k))
                    # 空字符串跳过
                    if column in self.ignore_value:
                        continue
                    # 只要有一个是字符串就全当字符串用
                    column_is_number, column_is_bool = isnumber(column), isbool(column)
                    if not column_is_number and not column_is_bool:
                        column_type[k] = 'string'
                    elif k not in column_type:
                        if column_is_number:
                            column_type[k] = 'number'
                        else:
                            column_type[k] = 'bool'
        except csv.Error as e:
            logger.warn('csv error near line %d' % self.reader.line_num)
            raise e
        # 2. 增加ename
        schema = {get_ename(k): {'cname': k, 'ename': get_ename(k), 'type': column_type[k]} for k in column_type}
        return schema

class NginxFormatter(BaseFormatter):
    '''将Nginx日志导入
注意会做一些url解析工作:
    1. 将$request解析，__request_method表示方法(GET), __request_path表示请求的path(去除参数),
       __request_query表示参数，__request_param_xx表示参数里面的xx。
    2. 将--url_fields指定的列认为是url并做解析，解析后会生成__<字段名>_<解析内容>这样命名的property,
       解析内容包括netloc, path, query, param_<参数明>。举例对于$my_url字段值为
       "http://www.abc.com/path/to/mine?k1=v1&k2=2", 会解析为
       {
           "__my_url_netloc": "www.abc.com",
           "__my_url_path": "/path/to/mine",
           "__my_url_query": "k1=v1&k2=v", 
           "__my_url_param_k1": "v1",
           "__my_url_param_k2": 2
       }

举例:
    1. format_importer.py nginx_event -l 'http://localhost:8006/sa' -ef '__http_referer_path' -df
       __request_param_cookieId -td '2014-08-01 12:00:00' -f access.log
       -F '$remote_addr - $remote_user [$time_local] "$request" $status +++$request_body+++ "$http_referer"'
       将access_log中所有列作为properties导入本地的私有部署版本，其中每行是一个事件，http_refer解析后
       的path对应的列为事件名，request解析后参数中的cookieId作为用户id, 时间全部为2014-08-01 12:00:00
    2. format_importer.py nginx_event -l 'http://localhost:8006/sa' -ed my_event -df
       __request_param_cookieId -tf time_local -fm "%d/%b/%Y:%H:%M:%S %z"
       -F '$remote_addr - $remote_user [$time_local] "$request" $status +++$request_body+++ "$my_url"'
       -pl 'status,__my_url_netloc' -uf my_url -fp '.*\.gif' -fp '.*\.png'
       将access_log中(status, http_refer解析后的netloc)作为properties导入本地的私有部署版本，
       过滤所有对gif,png的请求。对应事件名全部为my_event, 每行是一个事件, request解析后参数中的
       cookieId为distinct_id, time_local对应的列为time(格式类似22/Oct/2015:17:56:27 +0800), 
       并且将my_url字段作为url解析。
    3. format_importer.py nginx_profile -l 'http://localhost:8006/sa' -df __request_param_cookieId -f access.log 
       -F '$remote_addr - $remote_user [$time_local] "$request" $status +++$request_body+++ "$http_referer"
       将access_log中所有列作为用户属性导入本地的私有部署版本，其中每行是一个用户, request解析后参
       数中的cookieId作为用户id.
    4. format_importer.py nginx_event @./conf/nginx_event.conf
       也可以修改conf/nginx_event.conf和conf/nginx_profile.conf来填写所有必要参数
    '''
    def __init__(self, args):
        '''初始化方法'''
        super().__init__(args, args.filename)
        # 规范化
        property_list = args.property_list.split(',') if args.property_list else []
        skip_identify_list = args.skip_identify.split(',') if args.skip_identify else []
        self.url_fields = args.url_fields.split(',')
        self.filter_path_patterns = [re.compile(x) for x in args.filter_path]
        self.ip_from = args.ip_from
        self.ignore_value = args.ignore_value
        self.ignore_value.append('')
        logger.debug('ignore %s' % self.ignore_value)
        # 生成columns
        self.columns, self.log_format_pattern = compile_log_format(args.log_format)
        self.nginx_fd = open(args.filename, 'r')
        # 判断type 需要预读一段数据 因此判断完需要重置句柄
        self.column_type = self.__get_column_type(skip_identify_list, property_list)
        self.nginx_fd.close()
        self.nginx_fd = open(args.filename, 'r')

        logger.info('start import nginx, filename = %s, log_format = %s' % (args.filename, args.log_format))
        logger.info('columns = %s' % self.columns)
        logger.info('column_type: %s' % pprint.pformat(self.column_type, width=200))

    @staticmethod
    def add_parser(parser):
        '''初始化sub_parser'''
        parser.add_argument('--filename', '-f',
                help='必填,nginx日志文件路径',
                required=True)
        parser.add_argument('--log_format', '-F',
                help='必填,nginx日志配置，类似\'"$remote_addr" "$time_local" "$http_refer" "$status"\'',
                required=True)
        parser.add_argument('--property_list', '-pl',
                type=str,
                help='用逗号分割选取的property, ' \
                        '举例`-p http_refer,status`将会将http_refer和status两列作为property导入。' \
                        '如果不填写则表示全部作为property导入',
                default=None)
        parser.add_argument('--skip_identify', '-i',
                help='对应的列将不会做自动类型判断，' \
                        '举例`--skip_identify request_user,status`将会将request_user,status不做类型判断，'
                        '完全作为string导入。如果不填写则表示全部的选中列都会自动做类型判断')
        parser.add_argument('--url_fields', '-uf',
                help='对应的列将作为url解析，用逗号分割。解析后会生成__<字段名>' \
                    '_<解析内容>这样命名的property, 解析内容包括netloc, path, query, param_<参数明>' \
                    '。举例对于$my_url字段值为"http://www.abc.com/path/to/mine?k1=v1&k2=2", ' \
                    '会解析为{"__my_url_netloc": "www.abc.com", "__my_url_path": "/path/to/mine", ' \
                    '"__my_url_query": "k1=v1&k2=v", "__my_url_param_k1": "v1", "__my_url_param_k2": 2}' \
                    '注意可以再property_list配置这些字段。默认是"http_referer"',
                default='http_referer')
        parser.add_argument('--filter_path', '-fp',
                help='过滤对应的path，可多选。这里的path取的是$request的path.支持正则. 举例 ' \
                    '-fp \'.*\.gif\' -fp \'/index\.html\' 将过滤对gif的请求和index的请求过滤掉',
                action='append',
                default=[])
        parser.add_argument('--ip_from', '-if',
                help='哪个字段作为ip, 如果指定，则每条数据对应的ip为对应字段的值, 默认是$remote_addr',
                default='remote_addr')
        parser.add_argument('--ignore_value',
                action='append',
                default=[],
                help='指定某些值为空，比如指定 `--ignore_value null` 则所有的null都被认为是空值')

    def get_total_num(self):
        '''返回总条数'''
        return self.total_num

    def read_records(self):
        '''返回一行'''
        return self.__parse_nginx_log()

    def parse_property(self, record):
        '''解析properties'''
        properties = {}
        for column, column_type in self.column_type.items():
            if column not in record:
                continue
            # 空字符串跳过
            if record[column] in self.ignore_value:
                continue
            if column_type == 'number':
                properties[column] = float(record[column])
            elif column_type == 'bool':
                properties[column] = (record[column].lower() == 'true')
            else:
                properties[column] = record[column]
        if self.is_event:
            properties['$ip'] = record[self.ip_from]
        return properties

    def close(self):
        '''清理方法'''
        self.nginx_fd.close()
        logger.info('end import nginx')

    def __parse_nginx_log(self):
        '''按照pattern解析nginx log'''
        for line in self.nginx_fd.readlines():
            line = line.rstrip('\n')

            # 1. 正则解析Nginx日志
            m = self.log_format_pattern.match(line)
            if not m:
                logger.warn('invalid log line')
                logger.warn(line)
                raise Exception('log line not matched')
            values = m.groups()
            record = dict([(x, y) for x, y in zip(self.columns, values) if y])

            # 2. 对$request解析
            if 'request' in record:
                parse_result = parse_request(record['request'])
                record.update(parse_result)

            # 3. 过滤规则
            is_filter = False
            for p in self.filter_path_patterns:
                if p.match(record['__request_path']):
                    logger.debug('filter %s' % line)
                    is_filter = True
                    break
            if is_filter:
                continue

            # 4. 对内建的解析url
            for url_name in self.url_fields:
                if url_name not in record:
                    continue
                parse_result = parse_url(record[url_name])
                for k, v in parse_result.items():
                    if v:
                        record['__%s_%s' % (url_name, k)] = v

            yield record

    def __get_column_type(self, skip_identify_list, property_list):
        # 1. 遍历一遍，得到每一个列的类型，暂时只需要判断是数字还是字符串
        column_type = {}
        self.total_num = 0
        for record in self.__parse_nginx_log():
            self.total_num += 1
            # property_list如果用户没有指定 则认为全部导入
            if not property_list:
                property_list = record.keys()
            for k in property_list:
                if k not in record:
                    continue
                # 将用于选择event/distinc_id/time的列去掉
                if k in self.skip_cols:
                    continue
                # 用户指定不做自动检查的列
                if k in skip_identify_list:
                    column_type[k] = 'string'
                    continue
                column = record[k]
                if column in self.ignore_value:
                    continue
                # 只要有一个是字符串就全当字符串用
                column_is_number, column_is_bool  = isnumber(column), isbool(column)
                if not column_is_number and not column_is_bool:
                    column_type[k] = 'string'
                elif k not in column_type:
                    if column_is_number:
                        column_type[k] = 'number'
                    else:
                        column_type[k] = 'bool'
        return column_type

class MysqlFormatter(BaseFormatter):
    """提供sql，将mysql的数据导入
使用举例:
    1. format_importer.py mysql_event -l 'http://localhost:8006/sa' -ef event -df user -td '2014-08-01 12:00:00'
       -u root -p passwd -i localhost -P 1234 -d test_db 
       -q 'select event, user, col from event where d = '2014-08-01' order by id'
       查询mysql中event表，将col对应的列作为properties导入本地的私有部署版本，其中每行是一个事件，
       event字段对应的是事件名, user字段对应的是distinct_id, 时间全部为2014-08-01 12:00:00
    2. format_importer.py mysql_event -l 'http://localhost:8006/sa' -ed my_event -df user -tf time -fm '%Y%m%d%H%M%S'
       -q 'select user,time,col1,col2,col3 from event where d = '2014-08-01' order by id'
       查询mysql中event表，将(col1, col2, col3)作为properties导入本地的私有部署版本，
       对应事件名全部为my_event, 每行是一个事件，user对应的列为distinct_id,
       time对应的列为time(格式类似20150130125959)
    3. format_importer.py mysql_profile -l 'http://localhost:8006/sa' -df user 
       -u root -p passwd -i localhost -P 1234 -d test_db 
       -q 'select user, col from profile where d = '2014-08-01' order by id'
       查询mysql中profile表，将col对应的列作为用户属性导入本地的私有部署版本，其中每行是一个用户,
       user字段对应的是distinct_id
    4. format_importer.py mysql_event @./conf/mysql_event.conf
       也可以修改conf/mysql_event.conf和conf/mysql_profile.conf来填写所有必要参数
    """

    def __init__(self, args):
        '''初始化方法'''
        self.conn = pymysql.connect(
                host=args.host,
                port=args.port,
                user=args.user, 
                charset='utf8',
                passwd=args.password,
                db=args.db)
        if args.sql:
            self.sql = args.sql
        else:
            with open(args.filename) as f:
                self.sql = f.read()
        super().__init__(args, self.sql.replace('\t', ' ').replace('\n', ' '))
        self.cursor = self.conn.cursor()
        self.cursor.execute(self.sql)
        logger.info('start importing from mysql, sql=%s' % self.sql)
        self.total_num = self.cursor.rowcount

    @staticmethod
    def add_parser(parser):
        '''初始化sub_parser'''
        parser.add_argument('--user', '-u',
                help='必填，mysql的username',
                required=True)
        parser.add_argument('--password', '-p',
                help='必填，mysql的password',
                required=True)
        parser.add_argument('--host', '-i',
                help='必填，mysql的地址',
                required=True)
        parser.add_argument('--port', '-P',
                help='必填，mysql的端口号',
                type=int,
                required=True)
        parser.add_argument('--db', '-d',
                help='必填, mysql对应的db名',
                required=True)
        # sql必须要么传入整个sql 要么指定一个sql文件
        sql_group = parser.add_mutually_exclusive_group(required=True)
        sql_group.add_argument('--sql', '-q',
                help='和filename选一个必填，查询语句，建议加order by等方式保证多次查询结果顺序一致。')
        sql_group.add_argument('--filename', '-f',
                help='和sql选一个必填，查询语句所在的文件，建议加order by等方式保证多次查询结果顺序一致。')

    def get_total_num(self):
        '''返回总条数'''
        return self.total_num

    def read_records(self):
        '''返回一行'''
        columns = [x[0] for x in self.cursor.description]
        while True:
            row = self.cursor.fetchone()
            if not row:
                break
            # 去除空列
            record = dict([(x, y) for x, y in zip(columns, row) if y is not None])
            yield record

    def parse_property(self, record):
        '''解析properties'''
        properties = {}
        for k, v in record.items():
            if k in self.skip_cols:
                continue
            if v is None:
                continue
            if type(v) == decimal.Decimal:
                properties[k] = float(v)
            else:
                properties[k] = v
        return properties

    def close(self):
        '''清理方法'''
        self.cursor.close()
        self.conn.close()
        logger.info('end import from mysql.')


#######
# 
# main需要的
#
######
def main():
    args = parse_args()
    logger.debug('args: %s' % pprint.pformat(vars(args)))

    formatter_name = '%sFormatter' % args.subparser_name.split('_')[0].title()
    formatter = globals()[formatter_name](args)

    if args.debug:
        consumer = sensorsanalytics.DebugConsumer(args.url, False, 10000)
        logger.info('running debug mode')
    else:
        consumer = sensorsanalytics.BatchConsumer(args.url)
    sa = sensorsanalytics.SensorsAnalytics(consumer, args.project, True)
    sa.register_super_properties({'$lib': formatter.default_properties['$lib'], '$lib_version':
        formatter.default_properties['$lib_version']})
    sa._get_lib_properties = lambda:formatter.default_properties

    skip_lines_cnt_down = args.skip_cnt
    counter = {'total_write': 0, 'total_read': 0, 'error': 0, 'skip': args.skip_cnt, 'total': formatter.get_total_num()}
    error = False

    progress_interval = 10000 # 每1w条打一条进度
    for record in formatter.read_records():
        try:
            if counter['total_read'] % progress_interval == 0:
                percent = counter['total_read'] * 100.0 / counter['total']
                logger.info('progress %.2f%% %s' % (percent, counter))
            counter['total_read'] += 1
            if skip_lines_cnt_down > 0:
                skip_lines_cnt_down -= 1
                continue
            formatter.send(sa, record)
            counter['total_write'] += 1
        except Exception:
            counter['error'] += 1
            logger.warn('failed to parse line %d' % counter['total_read'])
            logger.warn(traceback.format_exc())
            if args.quit_on_error:
                error = True
                break

    sa.close()
    formatter.close()

    if error:
        logger.error('failed to import, please fix it and run with[--skip_cnt %d] again!' %
                counter['total_write'])
        return 1
    else:
        logger.info('counter = %s.' % counter)
        return 0

if __name__ == '__main__':
    sys.exit(main())
