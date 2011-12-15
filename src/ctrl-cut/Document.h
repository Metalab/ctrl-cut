#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include<libgen.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <string>
#include "stdint.h"
#include <iostream>
#include "boost/format.hpp"

#include "util/PJL.h"
#include "engrave/Engrave.h"
#include "cut/HPGLEncoder.h"
#include "engrave/PclEncoder.h"
#include "cut/model/CutModel.h"
#include "config/DocumentSettings.h"
#include "config/EngraveSettings.h"

#include "util/Util.h"
#include <cups/cups.h>
#include <cups/file.h>

#include "FileParser.h"
#include <boost/thread.hpp>
#include "svg/Svg2Ps.h"

using std::string;

class Document {
public:
  enum Format {
    UNSPECIFIED,
    POSTSCRIPT,
    SVG,
    VECTOR,
    PBM
  };

  enum LoadType {
    ENGRAVING,
    CUT,
    BOTH
  };

  typedef std::list<CutModel*> CutList;
  typedef std::list<Engraving*> EngraveList;
  typedef CutList::iterator CutIt;
  typedef EngraveList::iterator EngraveIt;
  typedef CutList::const_iterator CutConstIt;
  typedef EngraveList::const_iterator EngraveConstIt;

  DocumentSettings settings;
  CutList cutList;
  EngraveList engraveList;

  CutIt begin_cut() { return this->cutList.begin(); }
  CutConstIt begin_cut() const  { return this->cutList.begin(); }
  CutIt end_cut() { return this->cutList.end(); }
  CutConstIt end_cut() const  { return this->cutList.end(); }
  CutList::reference front_cut() { return this->cutList.front(); }
  CutList::reference back_cut() { return this->cutList.back(); }
  size_t size_cut() const { return this->cutList.size(); }
  bool empty_cut() const { return this->cutList.empty(); }

  EngraveIt begin_engrave() { return this->engraveList.begin(); }
  EngraveConstIt begin_engrave() const  { return this->engraveList.begin(); }
  EngraveIt end_engrave() { return this->engraveList.end(); }
  EngraveConstIt end_engrave() const  { return this->engraveList.end(); }
  EngraveList::reference front_engrave() { return this->engraveList.front(); }
  EngraveList::reference back_engrave() { return this->engraveList.back(); }
  size_t size_engrave() const { return this->engraveList.size(); }
  bool empty_engrave() const { return this->engraveList.empty(); }

  Document() {};
  virtual ~Document() {};

  void optimize();
  void addCut(CutModel* cut);
  void addRaster(Engraving* raster);
  CutList getCuts() { return cutList; }

  void write(std::ostream &out);
  void preprocess();

  Format findFormat(const string& filename) {
    string base = basename(strdup(filename.c_str()));
    string suffix = base.substr(base.rfind(".") + 1);
    transform ( suffix.begin(), suffix.end(), suffix.begin(), &Util::lower_case );

    if (suffix == "vector")
      return VECTOR;
    else if (suffix == "pbm")
      return PBM;
    else if (suffix == "svg")
      return SVG;
    else
      return POSTSCRIPT;
  }

  bool load(const string& filename, LoadType load = BOTH, Format docFormat = UNSPECIFIED) {
    typedef DocumentSettings DS;
    if(docFormat == UNSPECIFIED)
      docFormat = findFormat(filename);

    string base = basename(strdup(filename.c_str()));

    this->settings.put(DS::DATA_DIR, string(dirname(strdup(filename.c_str()))));
    this->settings.put(DS::BASENAME,base.erase(base.rfind(".")));

    cups_file_t* input_file;
    FileParser *parser = NULL;

    if (docFormat == POSTSCRIPT || docFormat == SVG) {
      if (docFormat == SVG) {
        int convertPipe[2];
        FILE *svgIn = fopen(filename.c_str(), "r");
        int svgFd = fileno (svgIn);

        if (pipe(convertPipe)) {
          fprintf(stderr, "Pipe failed.\n");
          return false;
        }

        Svg2Ps converter(svgFd, convertPipe[1]);
        boost::thread svg_converter_thread(&Svg2Ps::convert, converter);

        if ((input_file = cupsFileOpenFd(convertPipe[0], "r")) == NULL) {
          LOG_FATAL_MSG("unable to open print file", filename.c_str());
          return false;
        }
      } else if(docFormat == POSTSCRIPT){
        if ((input_file = cupsFileOpen(filename.c_str(), "r")) == NULL) {
          LOG_FATAL_MSG("unable to open print file", filename.c_str());
          return false;
        }
      }
      string file_basename = this->settings.get(DS::TEMP_DIR)+ "/" + this->settings.get(DS::BASENAME);

      // Write out the incoming cups data if debug is enabled.
      // FIXME: This is disabled for now since it has a bug:
      // If we're reading from network/stdin, and debug is on, we reopen
      // the dumped file as a FILE*. Otherwise, we'll keep the cups_file_t.
      // Subsequent code doesn't handle the difference.
  #if 0
      FILE *file_debug;
      FILE *file_cups;
      string filename_cups_debug;
      if (cc_loglevel >= CC_DEBUG) {
        /* We save the incoming cups data to the filesystem. */
        filename_cups_debug = file_basename + ".cups";
        file_debug = fopen(filename_cups_debug.c_str(), "w");

        /* Check that file handle opened. */
        if (!file_debug) {
          LOG_FATAL_MSG("Can't open", filename_cups_debug);
          return 1;
        }

        /* Write cups data to the filesystem. */
        int l;
        while ((l = cupsFileRead(input_file, buf, sizeof(buf))) > 0) {
          fwrite(buf, 1, l, file_debug);
        }
        fclose(file_debug);
        /* In case file_cups pointed to stdin we close the existing file handle
         * and switch over to using the debug file handle.
         */
        cupsFileClose(input_file);
        file_cups = fopen(filename_cups_debug, "r");
      }
  #endif

      PostscriptParser *psparser = new PostscriptParser(this->settings);
      // Uncomment this to force ghostscript to render to file using the ppmraw
      // backend, instead of in-memory rendering
      //    psparser->setRenderToFile(true);
      if (load == ENGRAVING || load == BOTH) {
        switch (this->settings.get(EngraveSettings::DITHERING)) {
        case EngraveSettings::DEFAULT_DITHERING:
          psparser->setRasterFormat(PostscriptParser::BITMAP);
          break;
        case EngraveSettings::BAYER:
        case EngraveSettings::FLOYD_STEINBERG:
        case EngraveSettings::JARVIS:
        case EngraveSettings::BURKE:
        case EngraveSettings::STUCKI:
        case EngraveSettings::SIERRA2:
        case EngraveSettings::SIERRA3:
          psparser->setRasterFormat(PostscriptParser::GRAYSCALE);
          break;

        default:
          assert(false);
        }
      }
      if (!psparser->parse(input_file)) {
        LOG_FATAL("Error processing postscript");
        return 1;
      }
      else {
        parser = psparser;
      }
    }

    if (load == ENGRAVING || load == BOTH) {
      Engraving *raster = NULL;
      if (docFormat == PBM) {
        raster = new Engraving(filename, this->settings);
      }
      else if (parser) {
        if (parser->hasBitmapData()) {
          LOG_DEBUG_STR("Processing bitmap data from memory");
          raster = new Engraving(parser->getImage(), this->settings);
        }
        else if (!parser->getBitmapFile().empty()) {
          raster = new Engraving(parser->getBitmapFile(), this->settings);
        }
      }
      if (raster) {
        this->addRaster(raster);
      }
    }

    CutModel *cut = NULL;
    if (load == CUT || load == BOTH) {
      if (docFormat == VECTOR) {
        cut = new CutModel(this->settings);
        if(!cut->load(filename))
          return false;
      }
      else if (parser) {
        cut = new CutModel(this->settings);
        if(!cut->load(parser->getVectorData()))
            return false;
      }
      if (cut) this->addCut(cut);
    }

    return true;
  }
};

#endif /* DOCUMENT_H_ */
