object Form1: TForm1
  Left = 190
  Top = 116
  Width = 361
  Height = 331
  Caption = 'BorJas'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Image1: TImage
    Left = 0
    Top = 33
    Width = 353
    Height = 264
    Align = alClient
    Stretch = True
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 353
    Height = 33
    Align = alTop
    TabOrder = 0
    object Label1: TLabel
      Left = 164
      Top = 12
      Width = 3
      Height = 13
    end
    object BitBtn1: TBitBtn
      Left = 4
      Top = 4
      Width = 75
      Height = 25
      Caption = 'Open'
      TabOrder = 0
      OnClick = BitBtn1Click
    end
    object BitBtn2: TBitBtn
      Left = 84
      Top = 4
      Width = 75
      Height = 25
      Caption = 'Save'
      TabOrder = 1
      OnClick = BitBtn2Click
    end
  end
  object OpenDialog1: TOpenDialog
    InitialDir = '.'
    Left = 220
    Top = 4
  end
  object SaveDialog1: TSaveDialog
    InitialDir = '.'
    Left = 252
    Top = 4
  end
end
