import { CSSProperties, useEffect, useRef } from 'react'

interface Props {
  style?: CSSProperties
}

export default function Owadview (props: Props) {
  const adviewDivRef = useRef<HTMLDivElement>(null)

  useEffect(() => {
    const adviewElement = document.createElement('owadview')
    adviewDivRef.current?.appendChild(adviewElement)
  })

  return <div style={props.style} ref={adviewDivRef}></div>
}
