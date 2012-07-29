/*============================================================================*/
/*
  VFLib: https://github.com/vinniefalco/VFLib

  Copyright (C) 2008 by Vinnie Falco <vinnie.falco@gmail.com>

  This library contains portions of other open source products covered by
  separate licenses. Please see the corresponding source files for specific
  terms.
  
  VFLib is provided under the terms of The MIT License (MIT):

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.
*/
/*============================================================================*/

void fillImage (Image dest,
                Point <int> destTopLeft,
                Image mask,
                Rectangle <int> maskBounds,
                BlendMode mode,
                double opacity,
                Colour colour)
{
  jassert (mask.getFormat () == Image::SingleChannel);
  jassert (mask.getBounds ().contains (maskBounds));

  Rectangle <int> const bounds = maskBounds.getIntersection (
    dest.getBounds () + destTopLeft);

  Pixels destPixels (dest, bounds);
  Pixels maskPixels (mask, bounds);

  switch (dest.getFormat ())
  {
  case Image::RGB:
    switch (mode)
    {
    case normal:      destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::normal> (colour, opacity)); break;
    case lighten:     destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::lighten> (colour, opacity)); break;
    case darken:      destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::darken> (colour, opacity)); break;
    case multiply:    destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::multiply> (colour, opacity)); break;
    case average:     destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::average> (colour, opacity)); break;
    case add:         destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::add> (colour, opacity)); break;
    case subtract:    destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::subtract> (colour, opacity)); break;
    case difference:  destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::difference> (colour, opacity)); break;
    case negation:    destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::negation> (colour, opacity)); break;
    case screen:      destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::screen> (colour, opacity)); break;
    case exclusion:   destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::exclusion> (colour, opacity)); break;
    case overlay:     destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::overlay> (colour, opacity)); break;
    case softLight:   destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::softLight> (colour, opacity)); break;
    case hardLight:   destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::hardLight> (colour, opacity)); break;
    case colorDodge:  destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::colorDodge> (colour, opacity)); break;
    case colorBurn:   destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::colorBurn> (colour, opacity)); break;
    case linearDodge: destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::linearDodge> (colour, opacity)); break;
    case linearBurn:  destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::linearBurn> (colour, opacity)); break;
    case linearLight: destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::linearLight> (colour, opacity)); break;
    case vividLight:  destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::vividLight> (colour, opacity)); break;
    case pinLight:    destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::pinLight> (colour, opacity)); break;
    case hardMix:     destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::hardMix> (colour, opacity)); break;
    case reflect:     destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::reflect> (colour, opacity)); break;
    case glow:        destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::glow> (colour, opacity)); break;
    case phoenix:     destPixels.iterate (maskPixels, Pixels::FillRGB_MaskOpacity <Pixels::Mode::phoenix> (colour, opacity)); break;
    default:
      jassertfalse;
    };
    break;

  case Image::SingleChannel:
  case Image::ARGB:
  default:
    jassertfalse;
    break;
  };
}

//------------------------------------------------------------------------------

void copyImage (Image dest,
                Point <int> destTopLeft,
                Image source,
                Rectangle <int> sourceBounds,
                BlendMode mode,
                double opacity)
{
  jassert (source.getBounds ().contains (sourceBounds));

  Rectangle <int> const bounds = sourceBounds.getIntersection (
    dest.getBounds () + destTopLeft);

  Pixels destPixels (dest, bounds);
  Pixels sourcePixels (source, bounds);

  switch (dest.getFormat ())
  {
  case Image::SingleChannel:
    switch (source.getFormat ())
    {
    case Image::SingleChannel:
      switch (mode)
      {
      case normal:      destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::normal> (opacity)); break;
      case lighten:     destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::lighten> (opacity)); break;
      case darken:      destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::darken> (opacity)); break;
      case multiply:    destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::multiply> (opacity)); break;
      case average:     destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::average> (opacity)); break;
      case add:         destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::add> (opacity)); break;
      case subtract:    destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::subtract> (opacity)); break;
      case difference:  destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::difference> (opacity)); break;
      case negation:    destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::negation> (opacity)); break;
      case screen:      destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::screen> (opacity)); break;
      case exclusion:   destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::exclusion> (opacity)); break;
      case overlay:     destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::overlay> (opacity)); break;
      case softLight:   destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::softLight> (opacity)); break;
      case hardLight:   destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::hardLight> (opacity)); break;
      case colorDodge:  destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::colorDodge> (opacity)); break;
      case colorBurn:   destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::colorBurn> (opacity)); break;
      case linearDodge: destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::linearDodge> (opacity)); break;
      case linearBurn:  destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::linearBurn> (opacity)); break;
      case linearLight: destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::linearLight> (opacity)); break;
      case vividLight:  destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::vividLight> (opacity)); break;
      case pinLight:    destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::pinLight> (opacity)); break;
      case hardMix:     destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::hardMix> (opacity)); break;
      case reflect:     destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::reflect> (opacity)); break;
      case glow:        destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::glow> (opacity)); break;
      case phoenix:     destPixels.iterate (sourcePixels, Pixels::BlendGray_Opacity <Pixels::Mode::phoenix> (opacity)); break;
      default:
        jassertfalse;
      };
      break;

    default:
      jassertfalse;
      break;
    };
    break;

  case Image::RGB:
    switch (source.getFormat ())
    {
    case Image::RGB:
      switch (mode)
      {
      case normal:      destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::normal> (opacity)); break;
      case lighten:     destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::lighten> (opacity)); break;
      case darken:      destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::darken> (opacity)); break;
      case multiply:    destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::multiply> (opacity)); break;
      case average:     destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::average> (opacity)); break;
      case add:         destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::add> (opacity)); break;
      case subtract:    destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::subtract> (opacity)); break;
      case difference:  destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::difference> (opacity)); break;
      case negation:    destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::negation> (opacity)); break;
      case screen:      destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::screen> (opacity)); break;
      case exclusion:   destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::exclusion> (opacity)); break;
      case overlay:     destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::overlay> (opacity)); break;
      case softLight:   destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::softLight> (opacity)); break;
      case hardLight:   destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::hardLight> (opacity)); break;
      case colorDodge:  destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::colorDodge> (opacity)); break;
      case colorBurn:   destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::colorBurn> (opacity)); break;
      case linearDodge: destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::linearDodge> (opacity)); break;
      case linearBurn:  destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::linearBurn> (opacity)); break;
      case linearLight: destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::linearLight> (opacity)); break;
      case vividLight:  destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::vividLight> (opacity)); break;
      case pinLight:    destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::pinLight> (opacity)); break;
      case hardMix:     destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::hardMix> (opacity)); break;
      case reflect:     destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::reflect> (opacity)); break;
      case glow:        destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::glow> (opacity)); break;
      case phoenix:     destPixels.iterate (sourcePixels, Pixels::BlendRGB_Opacity <Pixels::Mode::phoenix> (opacity)); break;
      default:
        jassertfalse;
      };
      break;

    case Image::ARGB:
      switch (mode)
      {
      case normal:      destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::normal> (opacity)); break;
      case lighten:     destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::lighten> (opacity)); break;
      case darken:      destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::darken> (opacity)); break;
      case multiply:    destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::multiply> (opacity)); break;
      case average:     destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::average> (opacity)); break;
      case add:         destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::add> (opacity)); break;
      case subtract:    destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::subtract> (opacity)); break;
      case difference:  destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::difference> (opacity)); break;
      case negation:    destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::negation> (opacity)); break;
      case screen:      destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::screen> (opacity)); break;
      case exclusion:   destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::exclusion> (opacity)); break;
      case overlay:     destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::overlay> (opacity)); break;
      case softLight:   destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::softLight> (opacity)); break;
      case hardLight:   destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::hardLight> (opacity)); break;
      case colorDodge:  destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::colorDodge> (opacity)); break;
      case colorBurn:   destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::colorBurn> (opacity)); break;
      case linearDodge: destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::linearDodge> (opacity)); break;
      case linearBurn:  destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::linearBurn> (opacity)); break;
      case linearLight: destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::linearLight> (opacity)); break;
      case vividLight:  destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::vividLight> (opacity)); break;
      case pinLight:    destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::pinLight> (opacity)); break;
      case hardMix:     destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::hardMix> (opacity)); break;
      case reflect:     destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::reflect> (opacity)); break;
      case glow:        destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::glow> (opacity)); break;
      case phoenix:     destPixels.iterate (sourcePixels, Pixels::BlendARGB_Opacity <Pixels::Mode::phoenix> (opacity)); break;
      default:
        jassertfalse;
      };
      break;

    default:
      jassertfalse;
    };
    break;

  default:
    jassertfalse;
    break;
  };
}
