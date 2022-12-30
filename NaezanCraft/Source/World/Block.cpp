#include "Block.h"

uint8_t Block::Get_rtf_AO()
{
    return RightAO & 0b11;
}

uint8_t Block::Get_rtb_AO()
{
    return (RightAO >> 2) & 0b11;
}

uint8_t Block::Get_rBf_AO()
{
    return (RightAO >> 4) & 0b11;
}

uint8_t Block::Get_rBb_AO()
{
    return (RightAO >> 6) & 0b11;
}

void Block::Set_rtf_AO(uint8_t ao)
{
    RightAO = (RightAO & 0b11111100) | (ao);
}

void Block::Set_rtb_AO(uint8_t ao)
{
    RightAO = (RightAO & 0b11110011) | (ao << 2);
}

void Block::Set_rBf_AO(uint8_t ao)
{
    RightAO = (RightAO & 0b11001111) | (ao << 4);
}

void Block::Set_rBb_AO(uint8_t ao)
{
    RightAO = (RightAO & 0b00111111) | (ao << 6);
}

uint8_t Block::Get_ltf_AO()
{
    return LeftAO & 0b11;
}

uint8_t Block::Get_ltb_AO()
{
    return (LeftAO >> 2) & 0b11;
}

uint8_t Block::Get_lBf_AO()
{
    return (LeftAO >> 4) & 0b11;
}

uint8_t Block::Get_lBb_AO()
{
    return (LeftAO >> 6) & 0b11;
}

void Block::Set_ltf_AO(uint8_t ao)
{
    LeftAO = (LeftAO & 0b11111100) | (ao);
}

void Block::Set_ltb_AO(uint8_t ao)
{
    LeftAO = (LeftAO & 0b11110011) | (ao << 2);
}

void Block::Set_lBf_AO(uint8_t ao)
{
    LeftAO = (LeftAO & 0b11001111) | (ao << 4);
}

void Block::Set_lBb_AO(uint8_t ao)
{
    LeftAO = (LeftAO & 0b00111111) | (ao << 6);
}

uint8_t Block::Get_trf_AO()
{
    return TopAO & 0b11;
}

uint8_t Block::Get_trb_AO()
{
    return (TopAO >> 2) & 0b11;
}

uint8_t Block::Get_tlf_AO()
{
    return (TopAO >> 4) & 0b11;
}

uint8_t Block::Get_tlb_AO()
{
    return (TopAO >> 6) & 0b11;
}

void Block::Set_trf_AO(uint8_t ao)
{
    TopAO = (TopAO & 0b11111100) | (ao);
}

void Block::Set_trb_AO(uint8_t ao)
{
    TopAO = (TopAO & 0b11110011) | (ao << 2);
}

void Block::Set_tlf_AO(uint8_t ao)
{
    TopAO = (TopAO & 0b11001111) | (ao << 4);
}

void Block::Set_tlb_AO(uint8_t ao)
{
    TopAO = (TopAO & 0b00111111) | (ao << 6);
}

uint8_t Block::Get_Brf_AO()
{
    return BottomAO & 0b11;
}

uint8_t Block::Get_Brb_AO()
{
    return (BottomAO >> 2) & 0b11;
}

uint8_t Block::Get_Blf_AO()
{
    return (BottomAO >> 4) & 0b11;
}

uint8_t Block::Get_Blb_AO()
{
    return (BottomAO >> 6) & 0b11;
}

void Block::Set_Brf_AO(uint8_t ao)
{
    BottomAO = (BottomAO & 0b11111100) | (ao);
}

void Block::Set_Brb_AO(uint8_t ao)
{
    BottomAO = (BottomAO & 0b11110011) | (ao << 2);
}

void Block::Set_Blf_AO(uint8_t ao)
{
    BottomAO = (BottomAO & 0b11001111) | (ao << 4);
}

void Block::Set_Blb_AO(uint8_t ao)
{
    BottomAO = (BottomAO & 0b00111111) | (ao << 6);
}

uint8_t Block::Get_frt_AO()
{
    return FrontAO & 0b11;
}

uint8_t Block::Get_frB_AO()
{
    return (FrontAO >> 2) & 0b11;
}

uint8_t Block::Get_flt_AO()
{
    return (FrontAO >> 4) & 0b11;
}

uint8_t Block::Get_flB_AO()
{
    return (FrontAO >> 6) & 0b11;
}

void Block::Set_frt_AO(uint8_t ao)
{
    FrontAO = (FrontAO & 0b11111100) | (ao);
}

void Block::Set_frB_AO(uint8_t ao)
{
    FrontAO = (FrontAO & 0b11110011) | (ao << 2);
}

void Block::Set_flt_AO(uint8_t ao)
{
    FrontAO = (FrontAO & 0b11001111) | (ao << 4);
}

void Block::Set_flB_AO(uint8_t ao)
{
    FrontAO = (FrontAO & 0b00111111) | (ao << 6);
}

uint8_t Block::Get_brt_AO()
{
    return BackAO & 0b11;
}

uint8_t Block::Get_brB_AO()
{
    return (BackAO >> 2) & 0b11;
}

uint8_t Block::Get_blt_AO()
{
    return (BackAO >> 4) & 0b11;
}

uint8_t Block::Get_blB_AO()
{
    return (BackAO >> 6) & 0b11;
}

void Block::Set_brt_AO(uint8_t ao)
{
    BackAO = (BackAO & 0b11111100) | (ao);
}

void Block::Set_brB_AO(uint8_t ao)
{
    BackAO = (BackAO & 0b11110011) | (ao << 2);
}

void Block::Set_blt_AO(uint8_t ao)
{
    BackAO = (BackAO & 0b11001111) | (ao << 4);
}

void Block::Set_blB_AO(uint8_t ao)
{
    BackAO = (BackAO & 0b00111111) | (ao << 6);
}
