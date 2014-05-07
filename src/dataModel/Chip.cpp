/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "Chip.h"

Chip::Chip()
    : m_id(""),
      m_barcodes(0),
      m_name(""),
      m_x1(0),
      m_x2(0),
      m_x1Total(0),
      m_x2Total(0),
      m_x1Border(0),
      m_x2Border(0),
      m_y1(0),
      m_y2(0),
      m_y1Total(0),
      m_y2Total(0),
      m_y1Border(0),
      m_y2Border(0)
{

}

Chip::Chip(const Chip& other)
{
    m_id = other.m_id;
    m_barcodes = other.m_barcodes;
    m_name = other.m_name;
    m_x1 = other.m_x1;
    m_x2 = other.m_x2;
    m_y1 = other.m_y1;
    m_y2 = other.m_y2;
    m_x1Border = other.m_x1Border;
    m_x2Border = other.m_x2Border;
    m_y1Border = other.m_y1Border;
    m_y2Border = other.m_y2Border;
    m_x1Total = other.m_x1Total;
    m_x2Total = other.m_x2Total;
    m_y1Total = other.m_y1Total;
    m_y2Total = other.m_y2Total;
}

Chip::~Chip()
{

}

Chip& Chip::operator=(const Chip& other)
{
    m_id = other.m_id;
    m_barcodes = other.m_barcodes;
    m_name  = other.m_name;
    m_x1 = other.m_x1;
    m_x2 = other.m_x2;
    m_y1 = other.m_y1;
    m_y2 = other.m_y2;
    m_x1Border = other.m_x1Border;
    m_x2Border = other.m_x2Border;
    m_y1Border = other.m_y1Border;
    m_y2Border = other.m_y2Border;
    m_x1Total = other.m_x1Total;
    m_x2Total = other.m_x2Total;
    m_y1Total = other.m_y1Total;
    m_y2Total = other.m_y2Total;
    return (*this);
}

bool Chip::operator==(const Chip& other) const
{
    return(
            m_id == other.m_id &&
            m_barcodes == other.m_barcodes &&
            m_name == other.m_name &&
            m_x1 == other.m_x1 &&
            m_x2 == other.m_x2 &&
            m_y1 == other.m_y1 &&
            m_y2 == other.m_y2 &&
            m_x1Border == other.m_x1Border &&
            m_x2Border == other.m_x2Border &&
            m_y1Border == other.m_y1Border &&
            m_y2Border == other.m_y2Border &&
            m_x1Total == other.m_x1Total &&
            m_x2Total == other.m_x2Total &&
            m_y1Total == other.m_y1Total &&
            m_y2Total == other.m_y2Total
        );
}
