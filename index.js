require ('hazardous')
const { spawn } = require('child_process')
const path = require('path')
const fs = require('fs')

module.exports = function (source, destination) {
  source = path.normalize(source)
  destination = path.normalize(destination)
  return new Promise((resolve, reject) => {
    if (process.platform.indexOf('win') !== 0) {
      reject(new Error('Only windows supported!'))
    }
    fs.access(source, fs.constants.F_OK, err => {
      if (err) {
        reject(new Error('Source file does not exist'))
      } else {
        const conv = spawn(path.resolve(__dirname, 'msoconv.exe'), [source, destination, 'pdf'])
        conv.on('close', code => {
          if (code === 0) {
            resolve(code)
          } else {
            reject(code)
          }
        })
      }
    })
  })
}

// excel: xlsx;xlsm;xlsb;xlam;xltx;xltm;xls;xlt;xla;xlm;xlw;odc;ods;prn;csv;dsn;mdb;mde;accdb;accde;dbc;iqy;dqy;rqy;oqy;cub;atom;atomsvc;dbf;xll;xlb;slk;dif;xlk;bak".split(";").indexOf(extension)!=-1){
// powerpoint pptx;ppt;pptm;ppsx;pps;ppsm;potx;pot;potm;odp;thmx;docx;docm;doc;ppam;ppa".split(";").indexOf(extension)!=-1){
// word docx;docm;dotx;dotm;doc;odt;docx;docm;doc;dotx;dotm;dotx;dotm;rtf;odt;doc;wpd;doc".split(";").indexOf(extension)!=-1){
