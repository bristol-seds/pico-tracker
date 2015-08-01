"""
Uploads payload_telemetry to habhub
"""

import base64
import hashlib
import couchdb
from datetime import datetime

couch = couchdb.Server('http://habitat.habhub.org/')
db = couch['habitat']

def habitat_upload(rx_time, packet_string):

    print packet_string

    # Packet ID
    packet_base64 = base64.standard_b64encode(packet_string+"\n")
    packet_sha256 = hashlib.sha256(packet_base64).hexdigest()

    # Time Created = backlog time
    time_created = rx_time.replace(microsecond=0).isoformat()+"+00:00"
    print time_created

    # Time Uploaded = now
    now = datetime.utcnow()
    time_uploaded = now.replace(microsecond=0).isoformat()+"+00:00"

    print db.save({
        "type":"payload_telemetry",
        "_id": packet_sha256,
        "data":{
            "_raw": packet_base64
        },
        "receivers": {
            "BACKLOG": {
                "time_created": time_created,
                "time_uploaded": time_created,
            }
        }
    })
