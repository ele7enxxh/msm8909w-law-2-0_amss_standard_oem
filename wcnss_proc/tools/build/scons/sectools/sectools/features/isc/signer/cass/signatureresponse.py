#Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
#Qualcomm Technologies Proprietary and Confidential.
from xml.etree.ElementTree import Element, SubElement, tostring
import base64
import os

BASE64_ENCODING = "Base64"

class SignatureResponse(object):
    def __init__(self, signatureResponseRoot, sequenceId, encoding):
        if (encoding != BASE64_ENCODING):
            raise RuntimeError, "SignatureResponse.init: Encoding %s is not supported" % encoding

        self.sequenceId = sequenceId
        self.root = signatureResponseRoot
        self.encoding = encoding

    def getTag(self, tag):
        node = self.root.find(tag)
        if (node is None):
            raise RuntimeError, "SignatureResponse.getTag(): %s in Signature Package does not exist!" % tag

        return node

    def saveResultToDir(self, dir):
        resultNode = self.getTag("Result")

        if resultNode.attrib['isEncoded'].lower() == 'true':
            result = base64.b64decode(resultNode.text)
        else:
            result = resultNode.text

        filename = resultNode.attrib['name']
        if not os.path.exists(dir):
            os.makedirs(dir)

        file = open(os.path.join(dir, filename), "wb")
        file.write(result)
        file.close()

    def getResult(self):
        resultNode = self.getTag("Result")

        if resultNode.attrib['isEncoded'].lower() == 'true':
            result = base64.b64decode(resultNode.text)
        else:
            result = resultNode.text

        return result

    def getStatusCode(self):
        return self.getTag("StatusCode").text

    def getError(self):
        return self.getTag("Error").text

    def release(self):
        return 1

    def __del__(self):
        self.release()
