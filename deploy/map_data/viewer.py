from bottle import route, run, response

import logging, os

from pymemcache.client import base

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    handlers=[
        # logging.FileHandler("debug.log"),
        logging.StreamHandler()
    ]
)

host = os.environ.get('MEMCACHED_HOST') or 'memcached'
port = os.environ.get('MEMCACHED_PORT') or 11211

cache = base.Client((host, port))


@route('/')
def alarm_map():
    response.content_type = 'application/json'
    return {
        'alerts': '/alert_statuses.json',
        'weather': '/weather_statuses.json',
        'explosives': '/explosives_statuses.json',
    }


@route('/alert_statuses.json')
def alarm_map():
    cached_data = cache.get('alarm_map')
    logging.info('caching data get: %s' % cached_data)
    response.content_type = 'application/json'
    return cached_data or {'error': 'no data in cache'}


@route('/weather_statuses.json')
def weather_map():
    cached_data = cache.get('weather_map')
    logging.info('caching data get: %s' % cached_data)
    response.content_type = 'application/json'
    return cached_data or {'error': 'no data in cache'}


@route('/explosives_statuses.json')
def weather_map():
    cached_data = cache.get('etryvoga_data')
    logging.info('caching data get: %s' % cached_data)
    response.content_type = 'application/json'
    return cached_data or {'error': 'no data in cache'}


logging.info('start viewer %s:%s' % (host, port))
run(host='0.0.0.0', port=8080, debug=True)
