const path = require('path')
const fs = require('fs')
const native = require('./build/Release/msoconv')

function getType (p, type) {
  if (type && type.length > 0) {
    return type
  }
  const ext = path.extname(p)
  if (ext && ext.length > 0) {
    if (ext.indexOf('.') === 0) {
      return ext.substr(1, ext.length - 1)
    }
    return ext
  }
}

function fsAccessExist (p) {
  return new Promise((resolve, reject) => {
    fs.access(p, fs.constants.F_OK, err => {
      if (err) {
        reject(err)
      } else {
        resolve()
      }
    })
  })
}

module.exports = async function (source, destination, type) {
  source = path.normalize(source)
  destination = path.normalize(destination)
  const destdir = path.dirname(destination)
  const promises = []
  promises.push(fsAccessExist(source))
  promises.push(fsAccessExist(destdir))

  await Promise.all(promises)

  return new Promise((resolve, reject) => {
    native.conv(source, destination, getType(destination, type), code => {
      if (code === 0) {
        resolve(code)
      } else {
        reject(code)
      }
    })
  })
}

// excel: xlsx;xlsm;xlsb;xlam;xltx;xltm;xls;xlt;xla;xlm;xlw;odc;ods;prn;csv;dsn;mdb;mde;accdb;accde;dbc;iqy;dqy;rqy;oqy;cub;atom;atomsvc;dbf;xll;xlb;slk;dif;xlk;bak".split(";").indexOf(extension)!=-1){
// powerpoint pptx;ppt;pptm;ppsx;pps;ppsm;potx;pot;potm;odp;thmx;docx;docm;doc;ppam;ppa".split(";").indexOf(extension)!=-1){
// word docx;docm;dotx;dotm;doc;odt;docx;docm;doc;dotx;dotm;dotx;dotm;rtf;odt;doc;wpd;doc".split(";").indexOf(extension)!=-1){
